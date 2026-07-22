
;; ================================================================================================
;; constants
;; ================================================================================================

UART_STATUS_RX_READY = $01
UART_STATUS_TX_READY = $02

DISK_CMD_NONE   = 0
DISK_CMD_READ   = 1
DISK_CMD_WRITE  = 2

DISK_STATUS_NONE  = 0
DISK_STATUS_BUSY  = 1
DISK_STATUS_READY = 2
DISK_STATUS_ERROR = 3

;; ================================================================================================
;; machine memory map
;; ================================================================================================

;; ZP (for booting only)
SRC         = $00
DST         = $02
STR         = $04
PTR         = $06
BUF         = $08

;; RAM
RAM_BASE     = $0000
BOOT         = $fa00

;; MMIO
DISK_BUF     = $fc00 ;; 512 bytes
DISK_STAT    = $fe00
DISK_CMD     = $fe01
DISK_LBA     = $fe02
UART_TX      = $fe10
UART_RX      = $fe11
UART_STAT    = $fe12
MMU_PAGE_TABLE     = $fe20 ;; 16 bytes

;; ROM
ROM_BASE     = $ff00
VECTORS      = $fffa
