        .org $0600       ; program load address

; --- Zero page variables ---
fib_lo      = $10    ; current fib low byte
fib_hi      = $11    ; current fib high byte
prev_lo     = $12    ; previous fib low byte
prev_hi     = $13    ; previous fib high byte
counter     = $14    ; iteration counter
temp_lo     = $15    ; temporary low byte
temp_hi     = $16    ; temporary high byte

; --- Initialize first two Fibonacci numbers ---
        LDA #0
        STA prev_lo
        STA prev_hi

        LDA #1
        STA fib_lo
        STA fib_hi

        LDA #20        ; we want Fib(20)
        STA counter

; --- Main loop ---
loop:
        DEC counter
        BEQ done

        ; new = fib + prev → store in temp
        LDA fib_lo
        CLC
        ADC prev_lo
        STA temp_lo
        LDA fib_hi
        ADC prev_hi
        STA temp_hi

        ; prev = fib
        LDA fib_lo
        STA prev_lo
        LDA fib_hi
        STA prev_hi

        ; fib = new
        LDA temp_lo
        STA fib_lo
        LDA temp_hi
        STA fib_hi

        JMP loop

; --- Store result ---
done:
        LDA fib_lo
        STA $0000
        LDA fib_hi
        STA $0001

        BRK             ; stop program
