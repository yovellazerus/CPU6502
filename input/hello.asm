
.include "arch0.inc"
.include "syscalls.inc"

.segment "USER"
.org user_entry     ;; my hack for now to "trick" ld65
                    ;; to put user labels in address for is intend RAM potions,
                    ;; and not relative to DISK
main:
    ldx #<hello_msg
    ldy #>hello_msg
    jsr _PUTS
    rts

hello_msg: .byte "Hello from a user program in the disk!", $0A, 0
