
ROM_READ  = $ff37
ROM_PUTC  = $ff7d
ROM_PRINT = $ff86

KERNEL_ENTRY = $c000
KERNEL_SIZE  = $3000

.segment "BOOT"
.org $0200
entry:

    lda #<msg_banner
    ldx #>msg_banner
    jsr ROM_PRINT

    lda #1          ; current LBA
    sta scb+2
    lda #0
    sta scb+3

    lda #<KERNEL_ENTRY
    sta scb+0
    lda #>KERNEL_ENTRY-$02
    sta scb+1

load_kernel:

    lda #<scb
    ldx #>scb
    jsr ROM_READ

    ; next LBA
    inc scb+2
    bne :+
    inc scb+3
:

    ; next destination buffer (+512)
    clc
    lda scb+0
    adc #$00
    sta scb+0

    lda scb+1
    adc #$02
    sta scb+1

    lda scb+1
    cmp #>(KERNEL_ENTRY + KERNEL_SIZE)
    bne load_kernel

    lda #<msg_to_kernel
    ldx #>msg_to_kernel
    jsr ROM_PRINT

to_kernel:
    jmp KERNEL_ENTRY
       
msg_banner:    .byte "**** boot loader v1.0: ****", $0A, 0
msg_to_kernel: .byte "bootloader: jumping to kernel...", $0A, 0

scb:
    .word KERNEL_ENTRY  ;; buffer
    .word 0             ;; lba

