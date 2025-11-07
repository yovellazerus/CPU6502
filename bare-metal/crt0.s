
;; cc65 compiler setup:

.export __STARTUP__

.export pusha
.export pushax
.export incsp1
.export incsp2

.import _main

.exportzp ptr1  = $00
.exportzp ptr2  = $02
.exportzp ptr3  = $04
.exportzp ptr4  = $06

.exportzp c_sp    = $08

; addeq0sp:
;         inc $0100
;         rts

pusha:
        dec c_sp
        bne @no_borrow
        dec c_sp+1
@no_borrow:
        ldy #$00
        sta (c_sp),y
        rts

pushax:
        dec c_sp
        bne @no_borrowA
        dec c_sp+1
@no_borrowA:
        ldy #$00
        sta (c_sp),y

        ; push X
        dec c_sp
        bne @no_borrowX
        dec c_sp+1
@no_borrowX:
        txa
        ldy #$00
        sta (c_sp),y
        rts

incsp1:
        inc c_sp
        bne @done1
        inc c_sp+1
@done1:
        rts

incsp2:
        inc c_sp
        bne @no_borrow1
        inc c_sp+1
@no_borrow1:
        inc c_sp
        bne @no_borrow2
        inc c_sp+1
@no_borrow2:
        rts

;; software stack:
SP_START_H   = $7F
SP_START_L   = $FF

;; from arch0:
mmio_power   = $C001
POWER_OFF    = $FF

.segment "STARTUP"

__STARTUP__:
    sei
    cld
    ldx #$FF
    txs
    lda #SP_START_H
    sta c_sp+1
    lda #SP_START_L
    sta c_sp+0
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
