.segment "STARTUP"

.global __STARTUP__
.import _main           ;; form main.c
.import zerobss         ;; form cc65
.import __STACK_START__ ;; form kernel.cfg
.importzp c_sp          ;; from cc65

__STARTUP__:

    sei
    cld
    ldx #$ff
    txs

    lda #<__STACK_START__
    sta c_sp + 0
    lda #>__STACK_START__
    sta c_sp + 1

    jsr zerobss

    jsr _main

    jmp *