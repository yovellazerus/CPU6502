
.include "arch0.inc"

bios_putchar = $FF90   ;; start of segment BIOS in the STARTUP ROM

.segment "BOOT"
.org boot_entry
_start:
    
    ldx #<hello_msg     ;; hello msg
    ldy #>hello_msg
    jsr  bios_putchar  

;; ****** writing to disk test ******

    ldx $FF             ;; copy from msg to disk data
@copy:
    lda DISK_DATA,x
    sta write_msg,x
    dex
    bne @copy 

    lda #0              ;; writing to disk test
    sta DISK_ADDRL
    lda #1
    sta DISK_ADDRH
    lda #DISK_WRITE
    sta DISK_CMD
@wait_disk:
    lda DISK_STATUS
    cmp #DISK_READY
    bne @wait_disk

    ;;**** load the KERNEL form the disk! ***;;

    .byte $FF

;; =====================================================================================================
;; BOOT read only segment
;; =====================================================================================================
        
hello_msg: .byte "**** boot loader v1.0: ****", $0A, 0
write_msg: .byte "I wrote this to disk.", $0A, 0

