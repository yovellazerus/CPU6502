
;; *****************************************************************************************************
;; This is a minimalist 6502 boot-strap and bios ROM
;; that boots a system from disk and forwards interrupts to kernel handlers, 
;; using a simple MMIO-based disk interface.
;; *****************************************************************************************************

.include "arch0.inc"

;; =====================================================================================================
;; BIOS startup for cold start and hot reset
;; =====================================================================================================

.segment "STARTUP"
bios_reset:
    sei             ;; init CPU registers 
    cld
    ldx #$FF
    txs

    lda #SCR_CLEAR  ;; clear the screen
    sta mmio_scr_ctrl

    ldx #<bios_msg
    ldy #>bios_msg
    jsr bios_putstring

    lda mmio_boot_status
    cmp #BOOT_FOUND
    beq @hot_reset

@cold_reset:
    ldx #<cold_msg
    ldy #>cold_msg
    jsr bios_putstring

    lda mmio_disk_status
    cmp #DISK_BAD
    bne @disk_found

    ldx #<error_msg     ;; no disk is found
    ldy #>error_msg
    jsr bios_putstring
@halt:
    jmp @halt

@disk_found:
    lda #0              ;; boot sector is block: $0000 of the disk
    sta mmio_disk_addrl
    sta mmio_disk_addrh
    lda #DISK_READ
    sta mmio_disk_cmd
@wait_disk:
    lda mmio_disk_status
    cmp #DISK_READY
    bne @wait_disk

    ldx $FF             ;; copy BOOT from disk data to it's entry point
@copy_boot:
    lda mmio_disk_data,x
    sta boot_entry,x
    dex
    bne @copy_boot

    lda #BOOT_FOUND
    sta mmio_boot_status

    ldx #<ready_msg
    ldy #>ready_msg
    jsr bios_putstring
    jmp boot_entry
@hot_reset:
    ldx #<hot_msg
    ldy #>hot_msg
    jsr bios_putstring
    jmp boot_entry

;; =====================================================================================================
;; BIOS routines and services to be use by the BOOT and first KERNEL stages
;; =====================================================================================================

.segment "BIOS"
;; void putstring(X: strL, Y: strH)
bios_putstring:
    stx rdi       
    sty rdi+1        
    ldy #0
@loop:
    lda (rdi),y
    beq @done   
    jsr bios_putchar
    iny             
    bne @loop  
    inc rdi+1        
    jmp @loop
@done:
    rts

;; void putchar(A : char)
bios_putchar:
    sta mmio_scr_data     
    lda #SCR_WRITE
    sta mmio_scr_ctrl
    rts

;; =====================================================================================================
;; BIOS read only segment
;; =====================================================================================================

bios_msg:        .byte "BIOS:", $0A, 0
cold_msg:        .byte "COLD START...", $0A, 0
hot_msg:         .byte "HOT RESET...", $0A, 0
ready_msg:       .byte "READY.", $0A, 0
error_msg:       .byte "ERROR: NO BOOTABLE DEVICE.", $0A, 0

;; =====================================================================================================
;; IRQ/BRK/NMI and RESET vectors
;; =====================================================================================================

.segment "VECTORS"
.word nmi_hook
.word bios_reset
.word irq_hook
