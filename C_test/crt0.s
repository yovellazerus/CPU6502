.segment "STARTUP"
.global __STARTUP__
.import _main

__STARTUP__:
    sei             ; disable interrupts
    ldx #$FF
    txs             ; set up stack pointer
    cld             ; clear decimal mode
    jsr _main       ; call main()
@loop:
    jmp @loop       ; infinite loop after main returns

irq:
nmi:
    lda #$FF
    sta $C001
    rti

.segment "VECTORS"
.word nmi
.word __STARTUP__
.word irq
