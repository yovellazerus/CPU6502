.segment "STARTUP"

.global __STARTUP__
.import _main

.import __STACK_START__
.importzp c_sp

.import __BSS_LOAD__
.import __BSS_SIZE__
.importzp ptr1
.importzp ptr2

__STARTUP__:

    lda #<__STACK_START__
    sta c_sp+0
    lda #>__STACK_START__
    sta c_sp+1

    lda #<__BSS_LOAD__
    sta ptr1
    lda #>__BSS_LOAD__
    sta ptr1+1

    lda #<__BSS_SIZE__
    sta ptr2
    lda #>__BSS_SIZE__
    sta ptr2+1

    lda #0
    ldy #0
bss_loop:
    sta (ptr1),y
    inc ptr1
    bne :+
    inc ptr1+1
:

    lda ptr1
    cmp ptr2
    lda ptr1+1
    sbc ptr2+1
    bcc bss_loop

    jsr _main

    jmp *