
;; *****************************************************************************************************
;; This is a minimalist 6502 boot-strap and bios ROM
;; that boots a system from disk and forwards interrupts to kernel handlers, 
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
    cmp #NO_DISK
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
    jsr print
    jmp *

nmi:
    pha
    txa
    pha
    tya
    pha
    tsx

    jmp (NMI_REG)

rom_nmi_return:

    sta MMU_MAP+15

    txs
    pla
    tay
    pla
    tax
    pla
    rti

irq:
    pha
    txa
    pha
    tya
    pha
    tsx

    lda $0103, x
    and #$10
    bne @irq
    jmp (BRK_REG)

@irq:
    jmp (IRQ_REG)

rom_irq_return:

    sta MMU_MAP+15

    txs
    pla
    tay
    pla
    tax
    pla
    rti

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

msg_banner:
  .byte "ROM:", $0a, 0

msg_boot:
  .byte "BOOTING...", $0a, 0

msg_no_disk:
  .byte "ERROR: NO BOOT", $0a, 0

boot_scb:
    .word BOOT  ;; buffer
    .word 0     ;; lba

.segment "VECTORS"
.word nmi
.word reset
.word irq
