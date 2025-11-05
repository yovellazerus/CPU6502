
;; cc65 compiler setup:

.export __STARTUP__

.export pusha
.export pushax
.export incsp1
.export incsp2
.export stax0sp

.import _main

.exportzp ptr1  = $00
.exportzp ptr2  = $02
.exportzp ptr3  = $04
.exportzp ptr4  = $06

.exportzp sp    = $08

; stax0sp:
;         ldy #$00          
;         sta (sp),y   
;         dec sp
;         bne @done
;         dec sp+1
; @done:
;         rts

stax0sp:
        ; push A
        dec sp
        bne @no_borrowA
        dec sp+1
@no_borrowA:
        ldy #$00
        sta (sp),y

        ; push X
        dec sp
        bne @no_borrowX
        dec sp+1
@no_borrowX:
        txa
        ldy #$00
        sta (sp),y

        rts



pusha:
        dec sp
        bne @no_borrow
        dec sp+1
@no_borrow:
        ldy #$00
        sta (sp),y
        rts

pushax:
        dec sp
        bne @no_borrowA
        dec sp+1
@no_borrowA:
        ldy #$00
        sta (sp),y

        ; push X
        dec sp
        bne @no_borrowX
        dec sp+1
@no_borrowX:
        txa
        ldy #$00
        sta (sp),y
        rts

incsp1:
        inc sp
        bne @done1
        inc sp+1
@done1:
        rts

incsp2:
        inc sp
        bne @no_borrow1
        inc sp+1
@no_borrow1:
        inc sp
        bne @no_borrow2
        inc sp+1
@no_borrow2:
        rts

;; from arch0:
mmio_power   = $C001
POWER_OFF    = $FF

.segment "STARTUP"

__STARTUP__:
    sei
    cld
    ldx #$ff
    txs
    jsr _main       ; call main()
Forever:
    jmp Forever

nmi_handler:
    lda #POWER_OFF
    sta mmio_power

irq_handler:
    rti

.segment "VECTORS"
.word nmi_handler
.word __STARTUP__        ; reset vector
.word irq_handler
