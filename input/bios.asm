
;; ***************************************************************************
;; This is a minimalist 6502 boot-strap and bios ROM
;; that boots a system from disk and forwards interrupts to kernel handlers, 
;; using a simple MMIO-based disk interface.
;; ***************************************************************************

DISK_READ  = 1
DISK_READY = 1
BOOT_YES   = 1
SCR_WRITE  = 1
SCR_CLEAR  = 2
KBD_READY  = 1

;; ***************************************************************************
;; MMIO registers
;; ***************************************************************************

;; kernel status
BOOT_STATUS  = $C003
;; dick ctrl
DISK_CMD     = $C004
DISK_ADDRL   = $C005
DISK_ADDRH   = $C006
DISK_STATUS  = $C007
;; I/O:
KEB_DATA     = $C008
KEB_CTRL     = $C009
SCR_DATA     = $C00A
SCR_CTRL     = $C00B

;; dick data
DISK_DATA    = $C100 ;; 256 byte blocks

;; ***************************************************************************
;; kernel fix entries
;; ***************************************************************************

boot_entry         = $C200
kernel_nmi         = $C300
kernel_irq         = $C400

.segment "BIOS"
bootstrap_reset:
    sei             ;; init CPU registers 
    cld
    ldx #$FF
    txs

    lda BOOT_STATUS
    cmp #BOOT_YES
    beq hot_reset
    
    lda #0          ;; boot sector is block: $0000 of the disk
    sta DISK_ADDRL
    sta DISK_ADDRH
    lda #DISK_READ
    sta DISK_CMD
wait_disk:
    lda DISK_STATUS
    cmp #DISK_READY
    bne wait_disk

    ldx $FF
copy_boot:
    lda DISK_DATA, x
    sta boot_entry, x
    dex
    bne copy_boot

    lda #BOOT_YES
    sta BOOT_STATUS
hot_reset:
    jmp boot_entry

.segment "VECTORS"
.word kernel_nmi
.word bootstrap_reset
.word kernel_irq
