.export _start
.import _main     ; <--- tell assembler main() comes from C file

.exportzp ptr1 = $00

.segment "STARTUP"

_start:
    ldx #$ff
    txs
    jsr _main       ; call main()
Forever:
    jmp Forever

.segment "VECTORS"
.word 0
.word _start        ; reset vector
.word 0
