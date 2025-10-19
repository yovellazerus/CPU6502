
;; ***************************************************************************
;; This is a minimalist 6502 BIOS
;; that boots a system from disk and forwards interrupts to kernel handlers, 
;; using a simple MMIO-based disk interface.
;; ***************************************************************************

PRL_KERNEL = 0
PRL_USER   = 1
DISK_READ  = 1
DISK_READY = 1
BOOT_YES   = 1

;; kernel fix entries
boot_entry   = $C1FF
kernel_nmi   = $C2FF
kernel_irq   = $C3FF
kernel_brk   = $C4FF

;; MMIO
PRL_REG      = $C000
BOOT_STATUS  = $C001
DISK_CMD     = $C002
DISK_ADDRL   = $C003
DISK_ADDRH   = $C004
DISK_STATUS  = $C005


DISK_DATA    = $C0FF ;; 256 byte blocks

;; NOTE: no memory protection is implemented for now in the CPU level, sow emulate via MMIO
;; all IRQ/NMI must return to bios! for memory protection implementation

.segment "BIOS"
bios_reset:
    sei             ;; init CPU registers 
    cld
    ldx #$FF
    txs

    lda #PRL_KERNEL ;; kernel mod
    sta PRL_REG

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

bios_nmi:
    pha
    txa
    pha
    tya
    pha

    lda #PRL_KERNEL ;; kernel mod
    sta PRL_REG

    jsr kernel_nmi

    lda #PRL_USER   ;; user mod
    sta PRL_REG

    pla
    tay
    pla
    tax
    pla
    rti

bios_irq:
    pha
    txa
    pha
    tya
    pha

    lda #PRL_KERNEL ;; kernel mod
    sta PRL_REG

    tsx            
    lda $0103, x
    and #$10       ;; test bit 4 (B flag)
    beq bios_irq_is_irq     

    jsr kernel_brk
    jmp bios_irq_end

bios_irq_is_irq:
    jsr kernel_irq

bios_irq_end:

    lda #PRL_USER   ;; user mod
    sta PRL_REG

    pla
    tay
    pla
    tax
    pla
    rti

.segment "VECTORS"
.word bios_nmi
.word bios_reset
.word bios_irq
