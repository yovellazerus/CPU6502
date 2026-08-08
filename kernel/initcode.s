.include "..\cc65-snapshot-win64\asminc\zeropage.inc"

.segment "INITCODE"
.org $0200
.global _init_code
_init_code:


_start:

    ;; init start message
    lda #<start_arg
    ldx #>start_arg
    ldy #'w'            
    brk
    nop  

    ;; sbrk test allocate 3 frames on the heap, for stress test
    ldy #'B'
    lda #$00
    ldx #$30
    brk
    nop 

    ;; fork the "shell"
    ldy #'F'
    brk
    nop
    cmp #0
    beq shell_code

init_loop:

    ;; wait for children
    ldy #'W'
    lda #<wait_arg
    ldx #>wait_arg
    brk
    nop

    sta pid+0
    stx pid+1

    cmp #$ff
    bne init_log
    cpx #$ff
    bne init_log

    ;; no running children
    jmp _start  

init_log:

    ;; "child: "
    lda #<prefix_log_arg
    ldx #>prefix_log_arg
    ldy #'w'            
    brk
    nop 

    ;; print pid
    lda pid+0
    ldx pid+1
    jsr print_hex16
    lda #' '
    jsr putchar 

    ;; "exited with code: "
    lda #<postfix_log_arg
    ldx #>postfix_log_arg
    ldy #'w'            
    brk
    nop 

    ;; print exit code
    lda wait_arg
    jsr print_hex8
    lda #$0a
    jsr putchar 

    jmp init_loop

shell_code:

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

    jmp shell_code

;; using The Double-Fork (Daemonization) Trick
@fork:
    ;; first fork
    ldy #'F'
    brk
    nop
    cmp #0
    beq @child_A

    ;; the Shell is here
    ;; it must wait for child A to exit so it doesn't become a zombie
@shell_wait:
    ldy #'W'
    lda #<wait_arg
    ldx #>wait_arg
    brk
    nop
    ;; Child A is reaped! Shell goes back to reading user input.
    jmp shell_code

@child_A:
    ;; second fork
    ;; we are child A, we immediately fork again to create the actual worker
    ldy #'F'
    brk
    nop
    cmp #0
    beq child_code

    ;; child A is here.
    ;; call exit
    ;; this orphans child B (worker) forcing the kernel to reparent child B to init
    
    lda #0         ;; exit code 0
    ldy #'E'  
    brk
    nop

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
    jmp shell_code

child_code:

    ;;rti
    
    lda #$0a
    jsr putchar

    ;;.byte $ff

    ;; write "PID: "
    lda #<pid_arg
    ldx #>pid_arg
    ldy #'w'           
    brk
    nop

    ;; sta $8000

    ;; getpid()
    ldy #'G'
    brk
    nop

    jsr print_hex8
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
    .word $0010
    .word $0000

pid:
    .word 0000

wait_arg:
    .word 0000

prompt_str: .byte "Shell: "
prompt_len = * - prompt_str

prefix_str: .byte "Echo: "
prefix_len = * - prefix_str

pid_str: .byte "PID: "
pid_len = * - pid_str

start_str: .byte "init: starting shell...", $0a, 0
start_len = * - start_str

prefix_log_str: .byte "child: ", 0
prefix_log_len = * - prefix_log_str

postfix_log_str: .byte "exited with code: ", 0
postfix_log_len = * - postfix_log_str

postfix_log_arg:
    .word 1             
    .word postfix_log_str    
    .word postfix_log_len 

prefix_log_arg:
    .word 1             
    .word prefix_log_str    
    .word prefix_log_len 

start_arg:
    .word 1             
    .word start_str    
    .word start_len   

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

