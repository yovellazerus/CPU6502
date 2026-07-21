
;; *****************************************************************************************************
;; This is a minimalist 6502 boot-strap and bios ROM
;; that boots a system from disk, 
;; using a simple MMIO-based disk interface.
;; *****************************************************************************************************

.include "machine.inc"

.segment "STARTUP"
reset:
    sei
    cld
    ldx #$ff
    txs

    lda #<msg_banner
    ldx #>msg_banner
    jsr print

    lda DISK_STAT
    cmp #DISK_STATUS_NONE
    beq bad_disk

    ;; ------ loading boot from disk -----

    lda #<boot_scb
    ldx #>boot_scb
    jsr read_sector
  
    ;; --------- jumping to boot --------

    lda #<msg_boot
    ldx #>msg_boot
    jsr print
    jmp BOOT  
bad_disk:
    lda #<msg_no_disk
    ldx #>msg_no_disk
    jmp error

nmi:
    lda #<msg_nmi
    ldx #>msg_nmi
    jmp error

irq:
    lda #<msg_irq
    ldx #>msg_irq
    jmp error

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
  
;;
;; void error(const char* msg)
;;
error:
    pha
    txa
    pha
    lda #<msg_error
    ldx #>msg_error
    jsr print
    pla
    tax
    pla
    jsr print
    jmp * ;; halt

msg_error:
  .byte "ERROR: ", 0

msg_nmi:
  .byte "NMI", $0a, 0

msg_irq:
  .byte "IRQ", $0a, 0

msg_banner:
  .byte "ROM:", $0a, 0

msg_boot:
  .byte "BOOTING...", $0a, 0

msg_no_disk:
  .byte "NO BOOT", $0a, 0

boot_scb:
    .word BOOT  ;; buffer
    .word 0     ;; lba

.segment "VECTORS"
.word nmi
.word reset
.word irq
