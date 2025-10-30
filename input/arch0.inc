;; ******************************************************************************************************
;; System V AMD64 ABI (x86-64 calling convention)
;; Registers: zero page registers x86-64 style (register size can be modify by the "REG_SIZE" constant)
;; Callee-saved: rbx, rbp, r12, r13, r14, r15
;; Caller-saved: rax, rcx, rdx, rsi, rdi, r8-r11
;; Args: rdi, rsi, rdx, rcx, r8, r9 (first 6 int/pointer)
;; Return: rax

;; EXCEPTIONS:
;; Stack: NO caller aligns to 16 bytes before call
;; Note: with documented exceptions to fit the 6502 architecture
;; ******************************************************************************************************

;; =====================================================================================================
;; macros:
;; =====================================================================================================

; copy size number of bytes from src to dest
.macro MOV src, dest, size
    ldx #0
@mov_loop:
    lda src,x
    sta dest,x
    inx
    cpx size
    bne @mov_loop
.endmacro


;; =====================================================================================================
;; consts:
;; =====================================================================================================

;; KERNEL constants

STACK_START_H    = $0F
STACK_START_L    = $FF

ERROR_UNDERFLOW  = $01
ERROR_UNKNOWN_MP = $02
ERROR_NO_ARG     = $03
NO_USER_PROGRAM  = $04
NO_FILE_FOUND    = $05

;; arch0 MMIO constants

DISK_READ    = 1
DISK_WRITE   = 2
DISK_READY   = 1
BOOT_FOUND   = 1
DISK_BAD     = 2
SCR_WRITE    = 1
SCR_CLEAR    = 2
KEB_READY    = 1
DISK_FOUND   = $FF
KEB_FOUND    = $FF
SCR_FOUND    = $FF
SPK_FOUND    = $FF
MIC_FOUND    = $FF
DISK_ERR     = 0
KEB_ERR      = 1
SCR_ERR      = 2
SPK_ERR      = 3
MIC_ERR      = 4
POWER_OFF    = $FF

;; =====================================================================================================
;; Zero page registers x86-64 style:
;; =====================================================================================================

ZP_X86_64 = $00
REG_SIZE  = 4

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
;; KERNEL zero page vars:
;; =====================================================================================================

KERNEL_ZP    = ZP_X86_64 + REG_SIZE*16

argc         = KERNEL_ZP + $00

;; =====================================================================================================
;; MMIO registers:
;; =====================================================================================================

;; wild-card devices
mmio_null         = $C000
mmio_power        = $C001
mmio_rnd          = $C002
;; KERNEL status
mmio_boot_status  = $C003
;; dick ctrl
mmio_disk_cmd     = $C004
mmio_disk_addrl   = $C005
mmio_disk_addrh   = $C006
mmio_disk_status  = $C007
;; I/O:
mmio_keb_data     = $C008
mmio_keb_ctrl     = $C009
mmio_scr_data     = $C00A
mmio_scr_ctrl     = $C00B
mmio_spk_data     = $C00C
mmio_spk_ctrl     = $C00D
mmio_mic_data     = $C00E
mmio_mic_ctrl     = $C00F
;; MMU
mmio_bsl          = $C010
mmio_bsh          = $C011
mmio_ssl          = $C012
mmio_ssh          = $C013

;; dick data
mmio_disk_data    = $C100 ;; 256 byte sectors

;; =====================================================================================================
;; kernel working RAM:
;; =====================================================================================================

user_entry   = $0200   ;; hard coded at the start of the RAM for now

KERNEL_RAM   = $C200

line         = KERNEL_RAM + $0000
argv         = KERNEL_RAM + $0100
nmi_hook     = KERNEL_RAM + $0200
irq_hook     = KERNEL_RAM + $0203
disk_buffer  = KERNEL_RAM + $0206
boot_entry   = KERNEL_RAM + $0306
root_dir     = KERNEL_RAM + $0406