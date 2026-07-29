.segment "INITCODE"
.org $0200
.global _init_code
_init_code:


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

init_wait_loop:
    ;; wait for children
    ldy #'W'
    lda #0
    ldx #0
    brk
    nop

    ;; sys_wait returns -1 ($FFFF) if there are no children left
    ;; A contains the low byte, X contains the high byte.
    cmp #$FF
    bne child_reaped
    cpx #$FF
    bne child_reaped
    
    ;; if we get here, sys_wait returned -1 mining: No children left
    jmp init_idle

child_reaped:
    ;; print a message acknowledging a child was reaped
    ldy #'P'
    lda #<reaped_arg
    ldx #>reaped_arg
    brk
    nop
    
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
    ldy #'P'
    lda #<child1_arg
    ldx #>child1_arg
    brk
    nop
    
    ;; exit(0)
    ldy #'E'
    lda #0  
    brk
    nop

;; =======================================================
;; child2 code 
;; =======================================================
child2_code:
    ldy #'P'
    lda #<child2_arg
    ldx #>child2_arg
    brk
    nop
    
    ;; exit(0)
    ldy #'E'
    lda #0  
    brk
    nop

;; =======================================================
;; data 
;; =======================================================

init_start_arg:
    .word (init_start_msg_end - init_start_msg)
    .word init_start_msg
init_start_msg:
    .byte "init: started and spawning children", $0a, 0
init_start_msg_end:

init_idle_arg:
    .word (init_idle_msg_end - init_idle_msg)
    .word init_idle_msg
init_idle_msg:
    .byte "init: just chilling here...", $0a, 0
init_idle_msg_end:

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

reaped_arg:
    .word (reaped_msg_end - reaped_msg)
    .word reaped_msg
reaped_msg:
    .byte "init: a child was reaped!", $0a, 0
reaped_msg_end: