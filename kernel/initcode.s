
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
    beq child

    ldy #'F'
    brk
    nop
    cmp #0
    bne init
    beq second

child:
    ldy #'F'
    brk
    nop
    cmp #0
    bne child_loop
    beq grandson

grandson:
    ldy #'P'
    lda #<grandson_arg
    ldx #>grandson_arg
    brk
    nop
    jmp grandson

second:
    ldy #'P'
    lda #<second_arg
    ldx #>second_arg
    brk
    nop
    jmp second

child_loop:
    ldy #'P'
    lda #<child_arg
    ldx #>child_arg
    brk
    nop
    jmp child_loop

init:
    ldy #'P'
    lda #<init_arg
    ldx #>init_arg
    brk
    nop
    jmp init

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
    
init_arg:
    .word (init_msg_end - init_msg)
    .word init_msg
init_msg:
    .byte "init", $0a, 0
init_msg_end:

second_arg:
    .word (second_msg_end - second_msg)
    .word second_msg
second_msg:
    .byte "second", $0a, 0
second_msg_end:
