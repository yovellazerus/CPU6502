
.include "..\cc65-snapshot-win64\asminc\zeropage.inc"

.segment "INITCODE"
.org $0200
.global _init_code
_init_code:

; _start:
;     ; 1. Print the prompt ("Enter text: ")
;     lda #<write_prompt_arg
;     ldx #>write_prompt_arg
;     ldy #'w'            ; SYS_WRITE
;     brk
;     nop                 ; Padding for standard 6502 BRK behavior

; read_loop:
;     ; 2. Read from stdin (fd = 0)
;     lda #<read_arg
;     ldx #>read_arg
;     ldy #'r'            ; SYS_READ
;     brk
;     nop

;     ; 3. Save the number of bytes read
;     ; The kernel returns the byte count in AX. 
;     ; We dynamically overwrite the 'size' field of our write_echo_arg struct.
;     sta write_echo_arg + 4  ; Low byte of size
;     stx write_echo_arg + 5  ; High byte of size

;     ; 4. Print the prefix ("You typed: ")
;     lda #<write_prefix_arg
;     ldx #>write_prefix_arg
;     ldy #'w'            ; SYS_WRITE
;     brk
;     nop

;     ; 5. Echo the user's input back to stdout (fd = 1)
;     lda #<write_echo_arg
;     ldx #>write_echo_arg
;     ldy #'w'            ; SYS_WRITE
;     brk
;     nop

;     ; 6. Loop back to the start
;     jmp _start


; ; ------------------------------------------------------------------
; ; System Call Argument Structs & Data
; ; ------------------------------------------------------------------

; prompt_str: .byte "Enter text: "
; prompt_len = * - prompt_str

; prefix_str: .byte "You typed: "
; prefix_len = * - prefix_str

; ; SyscallArg union structure: { int fd; void* buffer; uint16_t size; }
; ; Each field is 16-bit (2 bytes) in cc65.

; write_prompt_arg:
;     .word 1             ; fd = 1 (stdout)
;     .word prompt_str    ; buffer pointer
;     .word prompt_len    ; size

; read_arg:
;     .word 0             ; fd = 0 (stdin)
;     .word user_buffer   ; buffer pointer
;     .word 128           ; maximum size to read

; write_prefix_arg:
;     .word 1             ; fd = 1 (stdout)
;     .word prefix_str    ; buffer pointer
;     .word prefix_len    ; size

; write_echo_arg:
;     .word 1             ; fd = 1 (stdout)
;     .word user_buffer   ; buffer pointer
;     .word 0             ; size (populated dynamically at runtime)

; user_buffer: .res 128

;; =======================================================
;; init code
;; =======================================================
_start:

    ;; print init starting message
    ldy #'w'
    lda #<init_start_arg
    ldx #>init_start_arg
    brk
    nop

    ;; fork child 1
    ldy #'F'
    brk
    nop
    cmp #0
    beq child1_code

    ;; fork child 2
    ldy #'F'
    brk
    nop
    cmp #0
    beq child2_code

    ;; fork child 3
    ldy #'F'
    brk
    nop
    cmp #0
    beq child3_code

    ;; fork child 4
    ldy #'F'
    brk
    nop
    cmp #0
    beq child4_code

init_wait_loop:
    ;; wait for children
    ldy #'W'
    lda #<wait_status
    ldx #>wait_status
    brk
    nop

    ;; sys_wait returns -1 ($ffff) if there are no children left
    ;; A contains the low byte, X contains the high byte.
    cmp #$FF
    bne child_reaped
    cpx #$FF
    bne child_reaped
    
    ;; if we get here, sys_wait returned -1 mining: No children left
    jmp init_idle

child_reaped:
    ;; save the pid before any other syscalls overwrite A and X
    sta wait_pid+0
    stx wait_pid+1

    ;; print "init: reaped child PID 0x"
    ldy #'w'
    lda #<reaped_arg
    ldx #>reaped_arg
    brk
    nop

    ;; print the PID in hex
    lda wait_pid+0
    ldx wait_pid+1
    jsr print_hex16

    ;; print " with exit code 0x"
    ldy #'w'
    lda #<exit_code_arg
    ldx #>exit_code_arg
    brk
    nop

    ;; print the exit code in hex (wait_status + 0)
    lda wait_status + 0
    jsr print_hex8

    ;; print a newline character
    lda #$0a
    jsr putchar
    
    ;; loop back to wait for the next child
    jmp init_wait_loop

init_idle:
    ldy #'w'
    lda #<init_idle_arg
    ldx #>init_idle_arg
    brk
    nop
    jmp *

;; =======================================================
;; child1 code 
;; =======================================================
child1_code:

    ;; "watchdog" test
    rti

    ldy #'w'
    lda #<child1_arg
    ldx #>child1_arg
    brk
    nop
    
    ;; exit
    ldy #'E'
    lda #0
    brk
    nop

;; =======================================================
;; child2 code 
;; =======================================================
child2_code:

    ;; MMU test
    ; lda #$33
    ; sta $fe2f

    ldy #'w'
    lda #<child2_arg
    ldx #>child2_arg
    brk
    nop

    ;; invalid opcode test
    .byte $42
    
    ;; exit
    ldy #'E'
    lda #$42
    brk
    nop

;; =======================================================
;; child3 code 
;; =======================================================
child3_code:

    ;; "watchdog" test
    sei

    ldy #'w'
    lda #<child3_arg
    ldx #>child3_arg
    brk
    nop
    
    ;; exit
    ldy #'E'
    lda #0 
    brk
    nop

;; =======================================================
;; child4 code 
;; =======================================================
child4_code:
    ldy #'w'
    lda #<child4_arg
    ldx #>child4_arg
    brk
    nop

    ;; "watchdog" test
    ;;plp

    ;; sbrk() test
    ldy #'S'
    lda #$00
    ldx #$70
    brk
    nop

    ldy #'S'
    lda #$00
    ldx #$70
    brk
    nop

    ldy #'S'
    lda #$00
    ldx #$10
    brk
    nop

    ;; testing to write to the MMU register
    lda #$42
    sta $fe2f

    jmp *
    
    ;; exit
    ldy #'E'
    lda #$1
    brk
    nop

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

digits:
    .byte "0123456789abcdef"
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

putchar_arg:
    .word 1
    .word putchar_buffer
    .word (putchar_buffer_end - putchar_buffer)
putchar_buffer:
    .byte 0, 0
putchar_buffer_end:

;; =======================================================
;; init data 
;; =======================================================

init_input_arg:
    .word 0 
    .word init_input_buffer
    .word (init_input_buffer_end - init_input_buffer) 
init_input_buffer:
    .res 32
init_input_buffer_end:

init_start_arg:
    .word 1 
    .word init_start_msg 
    .word (init_start_msg_end - init_start_msg)
init_start_msg:
    .byte "init: started and spawning children", $0a, 0
init_start_msg_end:

wait_pid: 
    .res 2

wait_status: 
    .res 1

reaped_arg:
    .word 1
    .word reaped_msg
    .word (reaped_msg_end - reaped_msg)
reaped_msg:
    .byte "init: reaped child PID 0x", 0
reaped_msg_end:

exit_code_arg:
    .word 1
    .word exit_code_msg
    .word (exit_code_msg_end - exit_code_msg)
exit_code_msg:
    .byte " with exit code 0x", 0
exit_code_msg_end:

init_idle_arg:
    .word 1
    .word init_idle_msg
    .word (init_idle_msg_end - init_idle_msg)
init_idle_msg:
    .byte "init: just chilling here...", $0a, 0
init_idle_msg_end:

;; =======================================================
;; children data 
;; =======================================================

child1_arg:
    .word 1
    .word child1_msg
    .word (child1_msg_end - child1_msg)
child1_msg:
    .byte "child1: hello and goodbye", $0a, 0
child1_msg_end:

child2_arg:
    .word 1
    .word child2_msg
    .word (child2_msg_end - child2_msg)
child2_msg:
    .byte "child2: hello and goodbye", $0a, 0
child2_msg_end:

child3_arg:
    .word 1
    .word child3_msg
    .word (child3_msg_end - child3_msg)
child3_msg:
    .byte "child3: hello and goodbye", $0a, 0
child3_msg_end:

child4_arg:
    .word 1
    .word child4_msg
    .word (child4_msg_end - child4_msg)
child4_msg:
    .byte "child4: hello and goodbye", $0a, 0
child4_msg_end:
