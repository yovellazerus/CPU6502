
.include "arch0.inc"

.segment "BOOT"
_start:
    
    ldx #<hello_msg
    ldy #>hello_msg
    jsr puts
    lda #POWER_OFF
    sta POWER

;; void puts(X: strL, Y: strH)
puts:
    stx STR       
    sty STR+1        
    ldy #0
@loop:
    lda (STR),y
    beq @done   
    jsr putchar
    iny             
    bne @loop  
    inc STR+1        
    jmp @loop
@done:
    rts

;; void putchar(A : char)
putchar:
    sta SCR_DATA     
    lda #SCR_WRITE
    sta SCR_CTRL
    rts
    
hello_msg: .byte "Hello BOOT!", $0A, 0
