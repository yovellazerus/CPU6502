
.segment "CODE"
RESET:
    lda #$42
    sta $ff
    .byte $ff

nmi_handler:
    rti

irq_handler:
    rti

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
