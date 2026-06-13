
.include "common.inc"

.import __KERNEL_LOAD__
.import __KERNEL_SIZE__
.import __KERNEL_ENTRY__

.segment "CODE"

_start:
    lda #<msg_banner 
    ldx #>msg_banner
    jsr print
       
    jmp *

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


.segment "DATA"

msg_banner: .byte "**** kernel v1.0 ****", $0A, 0

.segment "RODATA"

.segment "BSS"

