
.org $8000

main:
    LDA #$42
    JSR foo
    STA $1000
    JMP halt

foo:
    RTS

halt:
    JMP halt

.org $fffa
.word nmi
.word RESET
.word irq
