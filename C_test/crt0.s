.segment "STARTUP"
.global __STARTUP__
.import _main

__STARTUP__:
    sei             ; disable interrupts
    cld             ; clear decimal mode
    ldx #$FF
    txs             ; set up stack pointer
    jsr _main       ; call main()
    jmp *

irq:
nmi:
    rti

.segment "VECTORS"
.word nmi
.word __STARTUP__
.word irq
