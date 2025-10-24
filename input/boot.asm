
.include "arch0.inc"

bios_putstring = $FF80   ;; start of segment BIOS in the STARTUP ROM

.segment "BOOT"
.org boot_entry
_start:
    
    ldx #<hello_msg     ;; hello msg
    ldy #>hello_msg
    jsr  bios_putstring  

    ;; ****** writing to disk test ******

    ldx $FF             ;; copy from msg to disk data
@copy:
    lda disk_data,x
    sta write_msg,x
    dex
    bne @copy 

    lda #0              ;; writing to disk test
    sta disk_addrl
    lda #1
    sta disk_addrh
    lda #DISK_WRITE
    sta disk_cmd
@wait_disk:
    lda disk_status
    cmp #DISK_READY
    bne @wait_disk

    ;;**** load the KERNEL form the disk! ***;;

    .byte $FF

;; =====================================================================================================
;; BOOT read only segment
;; =====================================================================================================
        
hello_msg: .byte "**** boot loader v1.0: ****", $0A, 0
write_msg: .byte "I wrote this to disk.", $0A, 0

