
.include "arch0.inc"

bios_putchar = $FF90   ;; start of segment BIOS in the STARTUP ROM

.segment "BOOT"
.org boot_entry
_start:
    
    ldx #<hello_msg     ;; hello msg
    ldy #>hello_msg
    jsr  bios_putchar   

    ;;**** load the KERNEL form the disk! ***;;

    .byte $FF

;; =====================================================================================================
;; BOOT read only segment
;; =====================================================================================================
        
hello_msg: .byte "**** boot loader v1.0: ****", $0A, 0

