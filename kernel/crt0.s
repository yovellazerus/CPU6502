
.segment "STARTUP"
.global __STARTUP__
.import _main

.import __STACK_START__
.importzp c_sp

__STARTUP__:
    
    lda #<__STACK_START__
    sta c_sp+0
    lda #>__STACK_START__
    sta c_sp+1

    jsr _main
    jmp *

