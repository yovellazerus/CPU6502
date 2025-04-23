
; parameters and return value for functions calling conventions
_a = $00
_b = $01
_c = $02
_d = $03
_e = $04
_f = $05
_g = $06
_h = $07
_i = $08
_j = $09

_2byte_parm0 = $00
_2byte_parm1 = $02
_2byte_parm2 = $04
_2byte_parm3 = $06

_4byte_parm0 = $00
_4byte_parm1 = $04

_ret_val_1byte = $0a
_ret_val_2byte = $0b
_ret_val_4byte = $0d
_ret_val_8byte = $11

.org $8000
_reset:
  cli
  jsr main ; user program
  jsr _hlt  ; hlt the CPU
  
main:
  lda #$42
  sta _a
  lda #$24
  sta _b
  jsr swap
  rts

swap:
  lda _a     ; load value from $10 into accumulator
  tax         ; transfer accumulator to x (temp storage)
  lda _b     ; load value from $11
  sta _a     ; store it into $10
  txa         ; transfer x (original $10) back to accumulator
  sta _b     ; store it into $11
  rts         ; return from subroutine
  
_hlt:
  jmp _hlt

.org $ff80
nmi_handler:
  rti
  
.org $ff00
irq_handler:
  rti

.org $fffa     
.word nmi_handler

.org $fffc     ; reset vector
.word _reset

.org $fffe 
.word irq_handler
