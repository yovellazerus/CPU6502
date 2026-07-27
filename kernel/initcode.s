
;; 
;; Machine code to bootstrap the first user space process.
;; Must not be called by the kernel!
;; for now, not real initcode implementation just the debug stage
;;

.segment "INITCODE"
.org $0200
.global _init_code
_init_code:
    
    ldy #'F'
    brk
    nop
    cmp #0
    bne parent
    beq child
    jmp *  ;; error

child:
    ldy #'F'
    brk
    nop
    cmp #0
    bne child_loop
    beq grandson
    jmp *

grandson:
    ldy #'D'
    lda #<grandson_arg
    ldx #>grandson_arg
    brk
    nop
    jmp grandson

child_loop:
    ldy #'D'
    lda #<child_arg
    ldx #>child_arg
    brk
    nop
    jmp child_loop

parent:
    ldy #'D'
    lda #<parent_arg
    ldx #>parent_arg
    brk
    nop
    jmp parent

grandson_arg:
    .word (grandson_msg_end - grandson_msg)
    .word grandson_msg
grandson_msg:
    .byte "grandson", $0a, 0
grandson_msg_end:

child_arg:
    .word (child_msg_end - child_msg)
    .word child_msg
child_msg:
    .byte "child", $0a, 0
child_msg_end:
    
parent_arg:
    .word (parent_msg_end - parent_msg)
    .word parent_msg
parent_msg:
    .byte "parent", $0a, 0
parent_msg_end:
