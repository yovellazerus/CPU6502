
;*************************************************************************
; ************************ my hex monitor :) *****************************
;*************************************************************************

;-------------------------------------------------------------------------
;  Constants
;-------------------------------------------------------------------------

POWER_OFF    = $FF
SCR_WRITE    = 1
SCR_CLEAR    = 2
KEB_READY    = 1

;-------------------------------------------------------------------------
;  zero-page registers
;-------------------------------------------------------------------------

TMP = $00

;-------------------------------------------------------------------------
;  Memory declaration
;-------------------------------------------------------------------------

LINE = $0200

;-------------------------------------------------------------------------
; MMIO register
;-------------------------------------------------------------------------

POWER        = $C001
KEB_DATA     = $C008
KEB_CTRL     = $C009
SCR_DATA     = $C00A
SCR_CTRL     = $C00B

;-------------------------------------------------------------------------
; the monitor
;-------------------------------------------------------------------------

.segment "LAZMON"
reset:
    cld
    cli
    ldx #$FF
    txs

    jsr printPrompt

main_loop:              ;; inf loop
    jsr getLine
    jsr parseLine
    jsr excLine
    jmp main_loop
 
;-------------------------------------------------------------------------
;  helper functions
;-------------------------------------------------------------------------

printPrompt:
    lda #'?'
    jsr putchar
    lda #' '
    jsr putchar
    rts

getLine:
    ldx #0
@loop:
    jsr waitchar
    pha
    cmp #$0D      ;; CR
    beq @end
    jsr putchar
    pla
    sta LINE,x
    inx
    beq @end
    jmp @loop
@end:
    lda #$0A
    jsr putchar
    rts

parseLine:
    rts

excLine:
    rts

;; void putstring(X: strL, Y: strH)
putstring:
    stx TMP       
    sty TMP+1        
    ldy #0
@loop:
    lda (TMP),y
    beq @done   
    jsr putchar
    iny             
    bne @loop  
    inc TMP+1        
    jmp @loop
@done:
    rts

;; void putchar(A : char)
putchar:
    sta SCR_DATA     
    lda #SCR_WRITE
    sta SCR_CTRL
    rts

;; void clear(void)
clear:
    pha
    lda #SCR_CLEAR
    sta SCR_CTRL
    pla
    rts

;; A : char waitchar()
waitchar:
    lda KEB_CTRL
    cmp #KEB_READY
    bne waitchar 
    lda #0
    sta KEB_CTRL
    lda KEB_DATA
    rts

;-------------------------------------------------------------------------
;  read only data segment
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
;  Vector area
;-------------------------------------------------------------------------

brk_handler:
    jmp reset           ;; BRK is going back to the monitor(end user program)

irq:
    pha
    txa
    pha
    tya
    pha

    tsx
    lda $0104,x
    and #$10            ;; test bit 4 (B flag)
    bne brk_handler

    ;; no real IRQ handler for now...

    pla
    tay
    pla
    tax
    pla
    rti

nmi:
    pha
    txa
    pha
    tya
    pha

    lda #POWER_OFF
    sta POWER

    pla
    tay
    pla
    tax
    pla
    rti

.segment "VECTORS"
.word nmi                       ;NMI vector
.word reset                     ;RESET vector
.word irq                       ;IRQ vector

;-------------------------------------------------------------------------
