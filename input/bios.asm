
;; *****************************************************************************************************
;; This is a minimalist 6502 boot-strap and bios ROM
;; that boots a system from disk and forwards interrupts to kernel handlers, 
;; using a simple MMIO-based disk interface.
;; *****************************************************************************************************

;; =====================================================================================================
;; constants
;; =====================================================================================================

DISK_READ    = 1
DISK_FOUND   = $FF
DISK_READY   = 1
BOOT_FOUND   = 1
SCR_WRITE    = 1
SCR_CLEAR    = 2
KEB_READY    = 1
KEB_FOUND    = $FF

;; =====================================================================================================
;; BIOS zp registers
;; =====================================================================================================

STR = $00

;; =====================================================================================================
;; MMIO registers
;; =====================================================================================================

;; KERNEL status
BOOT_STATUS  = $C003
;; dick ctrl
DISK_CMD     = $C004
DISK_ADDRL   = $C005
DISK_ADDRH   = $C006
DISK_STATUS  = $C007
;; I/O:
KEB_DATA     = $C008
KEB_CTRL     = $C009
SCR_DATA     = $C00A
SCR_CTRL     = $C00B

;; dick data
DISK_DATA    = $C100 ;; 256 byte blocks

;; =====================================================================================================
;; KERNEL fix entries
;; =====================================================================================================

boot_entry         = $C200
kernel_nmi         = $C300
kernel_irq         = $C400

;; =====================================================================================================
;; BIOS startup for cold start and hot reset
;; =====================================================================================================

.segment "BIOS"
bootstrap_reset:
    sei             ;; init CPU registers 
    cld
    ldx #$FF
    txs

    jsr bios_clear

    ldx #<bios_msg
    ldy #>bios_msg
    jsr bios_puts

    lda DISK_STATUS
    cmp DISK_FOUND
    beq @disk_found

    ldx #<no_disk_msg
    ldy #>no_disk_msg
    jsr bios_puts
    jmp bios_halt

@disk_found:
    lda BOOT_STATUS
    cmp #BOOT_FOUND
    beq @hot_reset

    ldx #<disk_msg
    ldy #>disk_msg
    jsr bios_puts

    lda KEB_CTRL
    cmp KEB_FOUND
    beq @keyboard_found

    ldx #<no_keyboard_msg
    ldy #>no_keyboard_msg
    jsr bios_puts
    jmp bios_halt

@keyboard_found:
    ldx #<keyboard_msg
    ldy #>keyboard_msg
    jsr bios_puts
    
    lda #0          ;; boot sector is block: $0000 of the disk
    sta DISK_ADDRL
    sta DISK_ADDRH
    lda #DISK_READ
    sta DISK_CMD
@wait_disk:
    lda DISK_STATUS
    cmp #DISK_READY
    bne @wait_disk

    ldx $FF
@copy_boot:
    lda DISK_DATA,x
    sta boot_entry,x
    dex
    bne @copy_boot

    lda #BOOT_FOUND
    sta BOOT_STATUS
@hot_reset:
    ldx #<boot_msg
    ldy #>boot_msg
    jsr bios_puts
    jmp boot_entry

;; =====================================================================================================
;; BIOS routines and services to be use by the BOOT and first KERNEL stages
;; =====================================================================================================

;; void puts(X: strL, Y: strH)
bios_puts:
    stx STR       
    sty STR+1        
    ldy #0
@loop:
    lda (STR),y
    beq @done   
    jsr bios_putchar
    iny             
    bne @loop  
    inc STR+1        
    jmp @loop
@done:
    rts

;; void putchar(A : char)
bios_putchar:
    sta SCR_DATA     
    lda #SCR_WRITE
    sta SCR_CTRL
    rts

;; void clear(void)
bios_clear:
    lda #SCR_CLEAR
    sta SCR_CTRL
    rts

;; A : char waitchar(void)
bios_waitchar:
    lda KEB_CTRL
    cmp #KEB_READY
    bne bios_waitchar 
    lda #0
    sta KEB_CTRL
    lda KEB_DATA
    rts

;; void halt(void)
bios_halt:
    jmp bios_halt

;; =====================================================================================================
;; BIOS msg strings, NOTE: consider removing it...
;; =====================================================================================================

bios_msg:        .byte "BIOS:", $0A, 0
disk_msg:        .byte "DISK OK", $0A, 0
keyboard_msg:    .byte "KEB OK", $0A, 0
boot_msg:        .byte "BOOT OK", $0A, "BIOS -> BOOT...", $0A, 0
no_disk_msg:     .byte "ERR: NO DISK", $0A, 0
no_keyboard_msg: .byte "ERR: NO KEB", $0A, 0

;; =====================================================================================================
;; IRQ/BRK/NMI and RESET vectors
;; =====================================================================================================

.segment "VECTORS"
.word kernel_nmi
.word bootstrap_reset
.word kernel_irq
