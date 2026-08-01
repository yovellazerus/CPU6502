
.include "..\cc65-snapshot-win64\asminc\zeropage.inc"
.include "../machine/machine.inc"

.import __KERNEL_START__
.import __KERNEL_SIZE__
.import __KERNEL_ENTRY__
.import __KERNEL_LBA__

.segment "BOOT"
entry:
    sei
    cld
    ldx #$ff
    txs

    lda #<msg_banner
    ldx #>msg_banner
    jsr prints

    lda #<msg_load_progress
    ldx #>msg_load_progress
    jsr prints

load_kernel:

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
    jsr prints

to_kernel:
    jmp __KERNEL_ENTRY__

;;
;; void read_sector(SCB* scb)
;;
read_sector:

    sta ptr1+0
    stx ptr1+1

    ldy #0
    lda (ptr1),y
    sta tmp1+0
    iny
    lda (ptr1),y
    sta tmp1+1
    iny
    lda (ptr1), y
    sta DISK_LBA+0
    iny
    lda (ptr1), y
    sta DISK_LBA+1

    lda #DISK_CMD_READ
    sta DISK_CMD

@wait:
    lda DISK_STAT
    cmp #DISK_STATUS_READY
    bne @wait

    lda #<DISK_BUF
    sta ptr2+0
    lda #>DISK_BUF
    sta ptr2+1

    ldy #0
@copy_low_page:
    lda (ptr2),y
    sta (tmp1),y
    iny
    bne @copy_low_page

    inc ptr2+1
    inc tmp1+1

    ldy #0
@copy_high_page:
    lda (ptr2),y
    sta (tmp1),y
    iny
    bne @copy_high_page
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
;; void prints(const char* str)
;;
prints:
  sta ptr1+0
  stx ptr1+1
  ldy #0
@loop:
  lda (ptr1),y
  beq @end
  jsr putchar
  iny
  bne @loop
@end:
  rts
       
msg_banner:          .byte "**** boot loader v1.0 ****", $0a, 0
msg_load_progress:   .byte "bootloader: loading kernel...", $0a, 0
msg_to_kernel:       .byte "bootloader: done!", $0a, "bootloader: jumping to kernel...", $0a, 0

scb:
    .word __KERNEL_START__   ;; buffer
    .word __KERNEL_LBA__     ;; lba

