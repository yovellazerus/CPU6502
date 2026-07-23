
.include "../machine/machine.inc"

.import __KERNEL_START__
.import __KERNEL_SIZE__
.import __KERNEL_ENTRY__
.import __KERNEL_LBA__

.segment "BOOT"
entry:

    lda #<msg_banner
    ldx #>msg_banner
    jsr print

    lda #<msg_load_progress
    ldx #>msg_load_progress
    jsr print

load_kernel:

    lda #'.'
    jsr putchar

    lda #<scb
    ldx #>scb
    jsr read_sector

    ; next LBA

    inc scb+2
    bne :+
    inc scb+3

:

    inc scb+1
    inc scb+1

    lda scb+1
    cmp #>(__KERNEL_START__ + __KERNEL_SIZE__)
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

    lda #DISK_CMD_READ
    sta DISK_CMD

wait:
    lda DISK_STAT
    cmp #DISK_STATUS_READY
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
  pha
@loop:
  lda UART_STAT
  and #UART_STATUS_TX_READY
  beq @loop
  pla
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
       
msg_banner:          .byte "**** boot loader v1.0 ****", $0a, 0
msg_load_progress:   .byte "loading kernel:", $0a, 0
msg_to_kernel:       .byte $0a, "done!", $0a, "bootloader: jumping to kernel...", $0a, 0

scb:
    .word __KERNEL_START__   ;; buffer
    .word __KERNEL_LBA__     ;; lba

