
;; DEFINES
NO_DISK   = 0
CMD_READ  = 1
STAT_READY = 2

;; ZP 
SRC         = $00
DST         = $02
STR         = $04
PTR         = $06
BUF         = $08

;; RAM
RAM_BASE     = $0000

BOOT         = $f000
INT_HOOKS    = $f000
NMI_REG      = INT_HOOKS+0
IRQ_REG      = INT_HOOKS+2
BRK_REG      = INT_HOOKS+4

;; MMIO
DISK_BUF     = $fc00 ;; 512 bytes
DISK_STAT    = $fe00
DISK_CMD     = $fe01
DISK_LBA     = $fe02
UART_TX      = $fe10
UART_RX      = $fe11
MMU_MAP      = $fe20
ROM_BASE     = $ff00
VECTORS      = $fffa