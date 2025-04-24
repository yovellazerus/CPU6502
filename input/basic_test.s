
val0 = $00
val1 = $01

.org $8000
reset:
  cli
  jsr main ; user program
  jsr hlt  ; hlt the CPU
  
main:
  lda #$42
  sta val0
  lda #$24
  sta val1
  jsr swap
  rts

swap:
  lda val0     ; load value from $10 into accumulator
  tax         ; transfer accumulator to x (temp storage)
  lda val1     ; load value from $11
  sta val0     ; store it into $10
  txa         ; transfer x (original $10) back to accumulator
  sta val1     ; store it into $11
  rts         ; return from subroutine
  
hlt:
  jmp hlt

.org $ff80
nmi_handler:
  rti
  
.org $ff00
irq_handler:
  rti

.org $fffa     
.word nmi_handler

.org $fffc     ; reset vector
.word reset

.org $fffe 
.word irq_handler
