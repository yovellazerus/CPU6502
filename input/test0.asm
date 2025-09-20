
ARG0 = $00
ARG1 = $01

TMP0 = $02

ERR  = $03

COUNTER = $04

keyboard_data   = $d010
keyboard_ctrl   = $d011
screen_data     = $d012
screen_ctrl     = $d013

.segment "CODE"
RESET:
    lda #$11
    sta ARG0
    lda #$22
    brk         ; test for brk puts the byte in ERR in zero-page
    .byte $42 
    sta ARG1
    jsr swap
    
main:
    lda keyboard_ctrl
    tax
    dex
    bne main

    lda keyboard_data
    sta screen_data
    lda #1
    sta screen_ctrl
    lda #0
    sta keyboard_ctrl
    jmp main

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
    pha         ; prologue
    txa
    pha
    tya
    pha

    ldx COUNTER
    inx
    stx COUNTER
    
    pla         ; epilogue
    tay
    pla
    tax
    pla
    rti

irq_handler:
    pha         ; prologue
    txa
    pha
    tya
    pha

    jmp brk_handler
    ; no real irq, for now only brk

irq_end:
    pla         ; epilogue
    tay
    pla
    tax
    pla
    rti

brk_handler:
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
    jmp irq_end

; void putchar(char)
bios_putchar:
    pha
    sta screen_data     
    lda #1
    sta screen_ctrl
    pla
    rts

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
