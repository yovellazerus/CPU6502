
DISK_READ  = 1
DISK_READY = 1
BOOT_YES   = 1

;; kernel fix entries
boot_entry   = $C1FF
kernel_nmi   = $C2FF
kernel_irq   = $C3FF
kernel_brk   = $C4FF

;; MMIO
BOOT_STATUS  = $C000
DISK_CMD     = $C001
DISK_ADDRL   = $C002
DISK_ADDRH   = $C003
DISK_STATUS  = $C004


DISK_DATA    = $C0FF ;; 256 byte blocks

;; NOTE: no memory protection is implemented for now
;; all IRQ/NMI must return to bios! for later  memory protection implementation

.segment "BIOS"
bios_reset:
    sei            ;; init CPU registers 
    cld
    ldx #$FF
    txs

    lda BOOT_STATUS
    cmp #BOOT_YES
    beq hot_reset
    
    lda #0         ;; boot sector is block: $0000 of the disk
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

    jsr kernel_nmi

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

    tsx            
    lda $0103, x
    and #$10       ;; test bit 4 (B flag)
    beq bios_irq_is_irq     

    jsr kernel_brk
    jmp bios_irq_end

bios_irq_is_irq:
    jsr kernel_irq

bios_irq_end:
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
