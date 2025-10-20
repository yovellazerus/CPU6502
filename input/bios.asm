
;; ***************************************************************************
;; This is a minimalist 6502 BIOS
;; that boots a system from disk and forwards interrupts to kernel handlers, 
;; using a simple MMIO-based disk interface.
;; ***************************************************************************

PRL_KERNEL = 0
PRL_USER   = 1
DISK_READ  = 1
DISK_READY = 1
BOOT_YES   = 1
SCR_WRITE  = 1
SCR_CLEAR  = 2
KBD_READY  = 1

;; ***************************************************************************
;; BIOS zp registers
;; ***************************************************************************

BIOS_TMP = $00

;; ***************************************************************************
;; MMIO registers
;; ***************************************************************************

;; MMU
PRL_REG      = $C000
BASE_REG     = $C001
SIZE_REG     = $C002
;; kernel status
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

;; ***************************************************************************
;; kernel fix entries
;; ***************************************************************************

boot_entry   = $C200
kernel_nmi   = $C300
kernel_irq   = $C400
kernel_brk   = $C500

;; NOTE: no memory protection is implemented for now in the CPU level, sow emulate via MMIO
;; all IRQ/NMI must return to bios! for memory protection implementation

.segment "BIOS"
bios_reset:
    sei             ;; init CPU registers 
    cld
    ldx #$FF
    txs

    lda #PRL_KERNEL ;; kernel mod
    sta PRL_REG

    lda BOOT_STATUS
    cmp #BOOT_YES
    beq hot_reset

    ldx #<cold_msg
    ldy #>cold_msg
    jsr bios_puts
    
    lda #0          ;; boot sector is block: $0000 of the disk
    sta DISK_ADDRL
    sta DISK_ADDRH
    lda #DISK_READ
    sta DISK_CMD
wait_disk:
    lda DISK_STATUS
    cmp #DISK_READY
    bne wait_disk

    ldx $FF
copy_boot:
    lda DISK_DATA, x
    sta boot_entry, x
    dex
    bne copy_boot

    lda #BOOT_YES
    sta BOOT_STATUS
hot_reset:
    ldx #<hot_msg
    ldy #>hot_msg
    jsr bios_puts
    jmp boot_entry

bios_nmi:
    pha
    txa
    pha
    tya
    pha

    lda #PRL_KERNEL ;; kernel mod
    sta PRL_REG

    jsr kernel_nmi

    lda #PRL_USER   ;; user mod
    sta PRL_REG

    pla
    tay
    pla
    tax
    pla
    rti

bios_irq:
    pha
    txa
    pha
    tya
    pha

    lda #PRL_KERNEL ;; kernel mod
    sta PRL_REG

    tsx            
    lda $0103, x
    and #$10       ;; test bit 4 (B flag)
    beq bios_irq_is_irq     

    jsr kernel_brk
    jmp bios_irq_end

bios_irq_is_irq:
    jsr kernel_irq

bios_irq_end:

    lda #PRL_USER   ;; user mod
    sta PRL_REG

    pla
    tay
    pla
    tax
    pla
    rti

;; ***************************************************************************
;; BIOS functions (A, X and Y are CALLER saved!) 
;; ***************************************************************************

;; void bios_puts(X: strL, Y: strH)
bios_puts:
    stx BIOS_TMP       
    sty BIOS_TMP+1        
    ldy #0
bios_puts_loop:
    lda (BIOS_TMP),y
    beq bios_puts_end   
    jsr bios_putchar
    iny             
    bne bios_puts_loop  
    inc BIOS_TMP+1        
    jmp bios_puts_loop
bios_puts_end:
    rts

;; void putchar(A : char)
bios_putchar:
    sta SCR_DATA     
    lda #SCR_WRITE
    sta SCR_CTRL
    rts

;; void bios_clear(void)
bios_clear:
    lda #SCR_CLEAR
    sta SCR_CTRL
    rts

;; A : char waitchar()
bios_waitchar:
    lda KEB_CTRL
    cmp #KBD_READY
    bne bios_waitchar 
    lda #0
    sta KEB_CTRL
    lda KEB_DATA
    rts

cold_msg:  .byte "BIOS: cold start...", $0A, 0
hot_msg:  .byte "BIOS: BOOT block FOUND, jumping to BOOT...", $0A, 0

.segment "VECTORS"
.word bios_nmi
.word bios_reset
.word bios_irq
