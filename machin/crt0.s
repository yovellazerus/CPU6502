.segment "STARTUP"
.global __STARTUP__
.import _main

__STARTUP__:
    jsr _main
    jmp *

