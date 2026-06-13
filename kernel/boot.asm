
.include "../machin/common.inc"

.import __KERNEL_LOAD__
.import __KERNEL_SIZE__
.import __KERNEL_ENTRY__

.segment "BOOT"
entry:

    lda #<msg_banner
    ldx #>msg_banner
    jsr print

    lda #1          ; current LBA
    sta scb+2
    lda #0
    sta scb+3

    lda #<__KERNEL_LOAD__
    sta scb+0
    lda #>__KERNEL_LOAD__
    sta scb+1

load_kernel:

    lda #<msg_load_progress1
    ldx #>msg_load_progress1
    jsr print

    lda scb+2
    jsr printHex8

    lda #' '
    jsr putchar

    lda #<msg_load_progress2
    ldx #>msg_load_progress2
    jsr print

    lda #>((__KERNEL_LOAD__ + __KERNEL_SIZE__) / 2 - 1)
    jsr printHex8

    lda #$0D
    jsr putchar

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
    cmp #>(__KERNEL_LOAD__ + __KERNEL_SIZE__)
    bne load_kernel

    lda #<msg_to_kernel
    ldx #>msg_to_kernel
    jsr print

to_kernel:
    jmp __KERNEL_ENTRY__

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

;;
;; void printHex8(unsigned char byte)
;;
printHex8:
    pha             
    lsr A           
    lsr A
    lsr A
    lsr A
    jsr printHex4 
    pla          
    and #$0F     
    jsr printHex4 
    rts

;;
;; void printHex4(unsigned char nibble)
;;
printHex4:
    tay             
    lda table_hex, y
    jsr putchar   
    rts

table_hex:
    .byte "0123456789ABCDEF"

       
msg_banner:          .byte "**** boot loader v1.0: ****", $0A, 0
msg_load_progress1:  .byte "loading sector:  $", 0
msg_load_progress2:  .byte "/ $", 0
msg_to_kernel:       .byte $0A, "done!", $0A, "bootloader: jumping to kernel...", $0A, 0

scb:
    .word __KERNEL_LOAD__   ;; buffer
    .word 0                 ;; lba

.segment "HOOK"
nmi_reg: .res 2
irq_reg: .res 2
brk_reg: .res 2
