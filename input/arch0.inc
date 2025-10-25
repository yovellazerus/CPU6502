
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

;; ******************************************************************************************************
;; System V AMD64 ABI (x86-64 calling convention)
;; Registers: zero page registers x86-64 style
;; Callee-saved: rbx, rbp, r12, r13, r14, r15
;; Caller-saved: rax, rcx, rdx, rsi, rdi, r8-r11
;; Args: rdi, rsi, rdx, rcx, r8, r9 (first 6 int/pointer)
;; Return: rax

;; EXCEPTIONS:
;; Stack: NO caller aligns to 16 bytes before call
;; Note: with documented exceptions to fit the 6502 architecture
;; ******************************************************************************************************


;; =====================================================================================================
;; Zero page registers x86-64 style:
;; =====================================================================================================

ZP_X86_64 = $00
REG_SIZE  = 8

rax = ZP_X86_64 + REG_SIZE*0 
rbx = ZP_X86_64 + REG_SIZE*1 
rcx = ZP_X86_64 + REG_SIZE*2 
rdx = ZP_X86_64 + REG_SIZE*3 
rsi = ZP_X86_64 + REG_SIZE*4 
rdi = ZP_X86_64 + REG_SIZE*5 
rbp = ZP_X86_64 + REG_SIZE*6 
rsp = ZP_X86_64 + REG_SIZE*7 
r8  = ZP_X86_64 + REG_SIZE*8 
r9  = ZP_X86_64 + REG_SIZE*9 
r10 = ZP_X86_64 + REG_SIZE*10 
r11 = ZP_X86_64 + REG_SIZE*11 
r12 = ZP_X86_64 + REG_SIZE*12 
r13 = ZP_X86_64 + REG_SIZE*13 
r14 = ZP_X86_64 + REG_SIZE*14 
r15 = ZP_X86_64 + REG_SIZE*15

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
