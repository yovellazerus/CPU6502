.include "..\cc65-snapshot-win64\asminc\zeropage.inc"

.segment "INITCODE"
.org $0200
.global _init_code
_init_code:

init_loop:

    ;; print the prompt
    lda #<write_prompt_arg
    ldx #>write_prompt_arg
    ldy #'w'            
    brk
    nop                 

read_loop:
    ;; read from stdin
    lda #<read_arg
    ldx #>read_arg
    ldy #'r'           
    brk
    nop

    sta write_echo_arg + 4
    stx write_echo_arg + 5

    ;; check for 'f'
    lda user_buffer + 0
    cmp #'f'
    beq @fork

    ;; check if character is < '0'
    cmp #'0'
    bcc @echo

    ;; check if character is >= ':'
    cmp #$3a
    bcs @echo

@kill_process:
    sec
    sbc #'0'
    ldx #0
    ldy #'K'
    brk
    nop

    jmp init_loop

@fork:
    ldy #'F'
    brk
    nop
    cmp #0
    beq child_code
    jmp init_loop

@echo:
    ; print the prefix ("Echo: ")
    lda #<write_prefix_arg
    ldx #>write_prefix_arg
    ldy #'w'     
    brk
    nop

    ;; echo the user input to stdout
    lda #<write_echo_arg
    ldx #>write_echo_arg
    ldy #'w'           
    brk
    nop

    ;; back to the start
    jmp init_loop

child_code:
    
    lda #$0a
    jsr putchar

    ;; write "PID: "
    lda #<pid_arg
    ldx #>pid_arg
    ldy #'w'           
    brk
    nop

    ;; getpid()
    ldy #'G'
    brk
    nop

    jsr print_hex4
    lda #$0a
    jsr putchar

    ;; sleep(sleep_arg)
    lda #<sleep_arg
    ldx #>sleep_arg
    ldy #'S'
    brk
    nop

    jmp child_code

;; ------------------------------------------------------------------
;; data
;; ------------------------------------------------------------------

sleep_arg:
    .word $0001
    .word $0000

prompt_str: .byte "Enter: "
prompt_len = * - prompt_str

prefix_str: .byte "Echo: "
prefix_len = * - prefix_str

pid_str: .byte "PID: "
pid_len = * - pid_str

pid_arg:
    .word 1
    .word pid_str
    .word pid_len

write_prompt_arg:
    .word 1             
    .word prompt_str    
    .word prompt_len    

read_arg:
    .word 0             
    .word user_buffer   
    .word 128           

write_prefix_arg:
    .word 1             
    .word prefix_str    
    .word prefix_len    

write_echo_arg:
    .word 1             
    .word user_buffer   
    .word 0             

user_buffer: .res 128

;; =======================================================
;; printing functions
;; =======================================================

;;
;; void print_hex16(uint16_t AX);
;;
print_hex16:
    pha             
    txa             
    jsr print_hex8  ;; print upper 8 bits
    pla             ;; 
    jmp print_hex8  ;; print lower 8 bits

;;
;; void print_hex8(uint8_t A);
;;
print_hex8:
    pha             
    lsr a
    lsr a
    lsr a
    lsr a
    jsr print_hex4
    pla 
    jmp print_hex4

;;
;; void print_hex4(uint8_t A);
;;
print_hex4:
    and #$0F        ;; mask out lower 4 bits
    stx tmp1
    tax             ;; transfer to x for indexing
    lda digits, x   ;; load ASCII character from table
    jsr putchar 
    ldx tmp1
    rts 

;;
;; void putchar(char A);
;;
putchar:
    sta putchar_buffer + 0
    ldy #'w'
    lda #<putchar_arg
    ldx #>putchar_arg
    brk
    nop
    rts

digits:
    .byte "0123456789abcdef"

putchar_arg:
    .word 1
    .word putchar_buffer
    .word 1

putchar_buffer: .res 1

