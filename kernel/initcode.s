
.include "..\cc65-snapshot-win64\asminc\zeropage.inc"

.segment "INITCODE"
.org $0200
.global _init_code
_init_code:

_start:

    ;; fork the child
    ldy #'F'
    brk
    nop
    cmp #0
    beq child_code

    ; ldy #'F'
    ; brk
    ; nop
    ; cmp #0
    ; beq child_code

    ; ldy #'F'
    ; brk
    ; nop
    ; cmp #0
    ; beq child_code

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

    ;; save the number of bytes read
    sta write_echo_arg + 4
    stx write_echo_arg + 5

    ; print the prefix ("You typed: ")
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
    lda #0
child_loop:
    pha
    ;; sleep
    ldy #'S'
    lda #<sleep_arg
    ldx #>sleep_arg
    brk
    nop
    pla

    ; new line
    pha
    lda #$0a
    jsr putchar
    pla

    ;; print the counter
    pha
    jsr print_hex8
    pla

    ;; increment the counter
    clc
    adc #1
    bne child_loop

    lda #$0a
    jsr putchar

    ldy #'E'
    lda #0
    brk
    nop

;; ------------------------------------------------------------------
;; data
;; ------------------------------------------------------------------

sleep_arg:
    .word $0010
    .word $0000

prompt_str: .byte "Enter text: "
prompt_len = * - prompt_str

prefix_str: .byte "You typed: "
prefix_len = * - prefix_str

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

