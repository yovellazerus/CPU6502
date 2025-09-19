
ARG0 = $00
ARG1 = $01

TMP0 = $02

ERR  = $03

KED   = $c000
DIS   = $c010

.segment "CODE"
RESET:
    lda #$11
    sta ARG0
    lda #$22
    brk         ; test for brk puts the byte in ERR in zero-page
    .byte $42 
    sta ARG1
    jsr swap
    jsr RESET   ; loop to halt the cpu

swap:
    pha         ; prologue

    lda ARG0
    sta TMP0    ; byte tmp = a
    lda ARG1
    brk
    .byte $33   ; test for brk puts the byte in ERR in zero-page
    sta ARG0    ; a = b
    lda TMP0
    sta ARG1    ; b = tmp

    pla         ; epilogue
    rts

nmi_handler:
    rti

; for now only brk handler 
irq_handler:
    pha         ; prologue
    txa
    pha
    tya
    pha

    tsx
    inx
    inx
    inx
    inx
    inx
    lda $0100,x
    tax
    dex
    lda RESET,x ; PCH is coded at RESET for now
    sta ERR

    pla         ; epilogue
    tay
    pla
    tax
    pla
    rti

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
