        .org = $8000        ; Start address of program in ROM

RESET:  LDX #$00         ; Load X with 0
LOOP:   STX $0200        ; Store X into RAM address $0200
        INX              ; Increment X
        LDY %10010011
        JMP LOOP         ; Jump back and repeat

; Reset vector (tells CPU where to start)
        .org = $FFFC
        .word RESET      ; Low byte, high byte of RESET address
        .word $0000      ; IRQ/BRK vector (not used here)
