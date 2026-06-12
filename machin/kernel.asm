
ROM_READ  = $ff37
ROM_PUTC  = $ff7d
ROM_PRINT = $ff86

KERNEL_ENTRY = $c000
KERNEL_SIZE  = $3000

.segment "KERNEL"
.org KERNEL_ENTRY
_start:
    lda #<msg_banner 
    ldx #>msg_banner
    jsr  ROM_PRINT
       
    jmp *

msg_banner: .byte "**** kernel v1.0 ****", $0A, 0

;; pad the kernel img to KERNEL_SIZE
.res KERNEL_SIZE - (* - KERNEL_ENTRY), 'k'
