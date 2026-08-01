.segment "INITCODE"
.org $0200
.global _init_code
_init_code:

.importzp ptr1
.importzp tmp1

.import __INITCODE_SIZE__

;; =======================================================
;; init code
;; =======================================================
_start:

    ;; print init starting message
    ldy #'P'
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
    ldy #'P'
    lda #<reaped_arg
    ldx #>reaped_arg
    brk
    nop

    ;; print the PID in hex
    lda wait_pid+0
    ldx wait_pid+1
    jsr print_hex16

    ;; print " with exit code 0x"
    ldy #'P'
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
    ldy #'P'
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

    ldy #'P'
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
    ; lda $fe27
    ; sta $fe20

    ldy #'P'
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

    ldy #'P'
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
    ldy #'P'
    lda #<child4_arg
    ldx #>child4_arg
    brk
    nop

    ;; "watchdog" test
    plp

    ;; sys_sbrk() test
    ; ldy #'S'
    ; lda #$00
    ; ldx #$70
    ; brk
    ; nop

    ; ldy #'S'
    ; lda #$00
    ; ldx #$70
    ; brk
    ; nop

    ; ldy #'S'
    ; lda #$00
    ; ldx #$10
    ; brk
    ; nop

    ; lda #$42
    ; sta $f000

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
    ldy #'P'
    lda #<putchar_arg
    ldx #>putchar_arg
    brk
    nop
    rts

putchar_arg:
    .word (putchar_buffer_end - putchar_buffer)
    .word putchar_buffer
putchar_buffer:
    .byte 0, 0
putchar_buffer_end:

;; =======================================================
;; init data 
;; =======================================================

init_start_arg:
    .word (init_start_msg_end - init_start_msg)
    .word init_start_msg
init_start_msg:
    .byte "init: started and spawning children", $0a, 0
init_start_msg_end:

wait_pid: 
    .res 2

wait_status: 
    .res 1

reaped_arg:
    .word (reaped_msg_end - reaped_msg)
    .word reaped_msg
reaped_msg:
    .byte "init: reaped child PID 0x", 0
reaped_msg_end:

exit_code_arg:
    .word (exit_code_msg_end - exit_code_msg)
    .word exit_code_msg
exit_code_msg:
    .byte " with exit code 0x", 0
exit_code_msg_end:

init_idle_arg:
    .word (init_idle_msg_end - init_idle_msg)
    .word init_idle_msg
init_idle_msg:
    .byte "init: just chilling here...", $0a, 0
init_idle_msg_end:

;; =======================================================
;; children data 
;; =======================================================

child1_arg:
    .word (child1_msg_end - child1_msg)
    .word child1_msg
child1_msg:
    .byte "child1: hello and goodbye", $0a, 0
child1_msg_end:

child2_arg:
    .word (child2_msg_end - child2_msg)
    .word child2_msg
child2_msg:
    .byte "child2: hello and goodbye", $0a, 0
child2_msg_end:

child3_arg:
    .word (child3_msg_end - child3_msg)
    .word child3_msg
child3_msg:
    .byte "child3: hello and goodbye", $0a, 0
child3_msg_end:

child4_arg:
    .word (child4_msg_end - child4_msg)
    .word child4_msg
child4_msg:
    .byte "child4: hello and goodbye", $0a, 0
child4_msg_end:
