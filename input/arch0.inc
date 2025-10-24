
;; *****************************************************************************************************
;; A heder file for my 6502 base computer aka arch0.
;; constants are UPPERCASE, vars and labels(functions) are lowercase.
;; *****************************************************************************************************


;; =====================================================================================================
;; constants
;; =====================================================================================================

DISK_IDLE    = 0
DISK_READ    = 1
DISK_WRITE   = 2

DISK_BAD     = 0
DISK_READY   = 1
DISK_ERR     = 2

BOOT_FOUND   = 1

SCR_WRITE    = 1
SCR_CLEAR    = 2

KEB_READY    = 1

DISK_PRESENT   = $FF
KEB_PRESENT    = $FF
SCR_PRESENT    = $FF
SPK_PRESENT    = $FF
MIC_PRESENT    = $FF

POWER_OFF    = $FF

;; =====================================================================================================
;; zp registers
;; =====================================================================================================

str = $00

;; =====================================================================================================
;; MMIO registers
;; =====================================================================================================

;; wild-card devices
null         = $C000
power        = $C001
rnd          = $C002
;; KERNEL status
boot_status  = $C003
;; dick ctrl
disk_cmd     = $C004
disk_addrl   = $C005
disk_addrh   = $C006
disk_status  = $C007
;; I/O:
keb_data     = $C008
keb_ctrl     = $C009
scr_data     = $C00A
scr_ctrl     = $C00B
spk_data     = $C00C
spk_ctrl     = $C00D
mic_data     = $C00E
mic_ctrl     = $C00F
;; MMU
bsl          = $C010
bsh          = $C011
ssl          = $C012
ssh          = $C013

;; dick data
disk_data    = $C100 ;; 256 byte blocks

;; =====================================================================================================
;; KERNEL fix entries
;; =====================================================================================================

boot_entry         = $C200
kernel_entry       = $C300
kernel_nmi         = $C400
kernel_irq         = $C500
