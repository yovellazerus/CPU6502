
ARG0 = 0x00
RET0 = 0x08

main:
    PHA
    JSR foo
    JSR _halt

_halt:
    JMP _halt

foo:
    LDA #0x42
    STA WPTR
    BEQ end
    INC WPTRH
end:
    RTS
