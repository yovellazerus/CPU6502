
ARG0 = $00
ARG1 = $01

TMP0 = $02

.segment "CODE"
RESET:
    lda #$11
    sta ARG0
    lda #$22
    sta ARG1
    jsr swap
    .byte $ff   ; halt for debug

; void swap(byte a, byte b)
; modify: (none) 
swap:
    pha ; prologue

    lda ARG0
    sta TMP0 ; byte tmp = a
    lda ARG1
    sta ARG0 ; a = b
    lda TMP0
    sta ARG1 ; b = tmp

    pla ; epilogue
    rts

nmi_handler:
    rti

irq_handler:
    rti

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
