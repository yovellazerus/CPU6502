
.include "common.inc"

.segment "BOOT"
.org BOOT
entry:

    lda #<msg_banner
    ldx #>msg_banner
    jsr print

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
    jsr read_sector

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
    jsr print

to_kernel:
    jmp KERNEL_ENTRY

;;
;; void read_sector(SCB* scb)
;;
read_sector:

    sta PTR+0
    stx PTR+1

    ldy #0
    lda (PTR),y
    sta BUF+0
    iny
    lda (PTR),y
    sta BUF+1
    iny
    lda (PTR), y
    sta DISK_LBA+0
    iny
    lda (PTR), y
    sta DISK_LBA+1

    lda #CMD_READ
    sta DISK_CMD

wait:
    lda DISK_STAT
    cmp #STAT_READY
    bne wait

    lda #<DISK_BUF
    sta SRC+0
    lda #>DISK_BUF
    sta SRC+1

    ldy #0
copy_low_page:
    lda (SRC),y
    sta (BUF),y
    iny
    bne copy_low_page

    inc SRC+1
    inc BUF+1

    ldy #0
copy_high_page:
    lda (SRC),y
    sta (BUF),y
    iny
    bne copy_high_page
    rts

;;
;; void putchar(char c)
;;
putchar:
  ldx UART_TX
  bne putchar
  sta UART_TX
  rts

;;
;; void print(const char* str)
;;
print:
  sta STR+0
  stx STR+1
  ldy #0
print_loop:
  lda (STR),y
  beq print_end
  jsr putchar
  iny
  bne print_loop
print_end:
  rts
       
msg_banner:    .byte "**** boot loader v1.0: ****", $0A, 0
msg_to_kernel: .byte "bootloader: jumping to kernel...", $0A, 0

scb:
    .word KERNEL_ENTRY  ;; buffer
    .word 0             ;; lba
