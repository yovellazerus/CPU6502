
;; *****************************************************************************************************
;; This is a minimalist 6502 boot-strap and bios ROM
;; that boots a system from disk and forwards interrupts to kernel handlers, 
;; using a simple MMIO-based disk interface.
;; *****************************************************************************************************

.include "arch0.inc"

;; =====================================================================================================
;; BIOS startup for cold start and hot reset
;; =====================================================================================================

.segment "BIOS"
bios_reset:
    sei             ;; init CPU registers 
    cld
    ldx #$FF
    txs

    lda #SCR_CLEAR  ;; clear the screen
    sta SCR_CTRL

    ldx #<bios_msg
    ldy #>bios_msg
    jsr bios_puts

    lda BOOT_STATUS
    cmp #BOOT_FOUND
    beq @hot_reset

@cold_reset:
    ldx #<cold_msg
    ldy #>cold_msg
    jsr bios_puts

    lda DISK_STATUS
    cmp DISK_FOUND
    beq @disk_found

    ldx #<error_msg     ;; no disk is found
    ldy #>error_msg
    jsr bios_puts
    lda #POWER_OFF
    sta POWER

@disk_found:

    lda #0              ;; boot sector is block: $0000 of the disk
    sta DISK_ADDRL
    sta DISK_ADDRH
    lda #DISK_READ
    sta DISK_CMD
@wait_disk:
    lda DISK_STATUS
    cmp #DISK_READY
    bne @wait_disk

    ldx $FF             ;; copy BOOT from disk buffer to it's entry point
@copy_boot:
    lda DISK_DATA,x
    sta boot_entry,x
    dex
    bne @copy_boot

    lda #BOOT_FOUND
    sta BOOT_STATUS

    ldx #<ready_msg
    ldy #>ready_msg
    jsr bios_puts
    jmp boot_entry
@hot_reset:
    ldx #<hot_msg
    ldy #>hot_msg
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

;; =====================================================================================================
;; BIOS msg strings, NOTE: consider removing it...
;; =====================================================================================================

bios_msg:        .byte "BIOS:", $0A, 0
cold_msg:        .byte "COLD START...", $0A, 0
hot_msg:         .byte "HOT RESET...", $0A, 0
ready_msg:       .byte "READY.", $0A, 0
error_msg:       .byte "ERR: NO BOOT.", $0A, 0

;; =====================================================================================================
;; IRQ/BRK/NMI and RESET vectors
;; =====================================================================================================

.segment "VECTORS"
.word kernel_nmi
.word bios_reset
.word kernel_irq
