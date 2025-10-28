
;********************************************************************************************************
;*************               ROM BASE OS - Core Firmware for the 6502           *************************
;********************************************************************************************************

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
    cpx #size
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

;; arch0 MMIO constants

DISK_READ    = 1
DISK_READY   = 1
BOOT_FOUND   = 1
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
disk_data    = $C100 ;; 256 byte sectors

;; =====================================================================================================
;; kernel working RAM:
;; =====================================================================================================

KERNEL_RAM   = $C200

line         = KERNEL_RAM + $0000
argv         = KERNEL_RAM + $0100
nmi_hook     = KERNEL_RAM + $0200
irq_hook     = KERNEL_RAM + $0203

;; =====================================================================================================
;;; kernel read only data is here:
;; =====================================================================================================

.segment "RODATA"

welcome_msg:            .byte "**** welcome to the 6502 kernel monitor ****", $0A, 0
prompt_msg:             .byte "> ", 0
user_exit_msg:          .byte "program exited with code: ", 0

error_prefix_msg:       .byte "ERROR: ", 0
error_underFlow_msg:    .byte "underFlow", 0
error_unknown_mp_msg:   .byte "unknown monitor service", 0
error_no_arg_msg:       .byte "no arguments were provided", 0

mp_dump_str:            .byte "dump", 0
mp_clear_str:           .byte "clear", 0
mp_exit_str:            .byte "exit", 0
mp_run_str:             .byte "run", 0

.segment "LIB"

;; void sys_putchar(char c)
;; input in A
sys_putchar:
    pha
    sta scr_data     
    lda #SCR_WRITE
    sta scr_ctrl
    pla
    rts

;; void sys_clear(void)
sys_clear:
    pha
    lda #SCR_CLEAR
    sta scr_ctrl
    pla
    rts

;; char sys_waitchar()
;; output in A
sys_waitchar:
    lda keb_ctrl
    cmp #KEB_READY
    bne sys_waitchar 
    lda #0
    sta keb_ctrl
    lda keb_data
    rts

;; void sys_puts(str s) 
;; input in X, Y
sys_puts:
    stx r8       
    sty r8+1        
    ldy #0
@loop:
    lda (r8),y
    beq @done  
    jsr sys_putchar
    iny             
    bne @loop  
    inc r8+1        
    jmp @loop
@done:
    rts

;; byte sys_strcmp(str a, str b)
;; note: 0 if eq 1 if diff no <,> for now
sys_strcmp:
    lda rdi
    sta r8
    lda rdi+1
    sta r8+1

    lda rsi
    sta r9
    lda rsi+1
    sta r9+1

@loop:
    ldy #0
    lda (r8),Y
    cmp #0
    beq @null       
    sta r10           
    lda (r9),Y     
    cmp r10       
    bne @diff                   
    inc r8
    bne @skip0
    inc r8+1
@skip0:
    inc r9
    bne @skip1
    inc r9+1
@skip1:
    jmp @loop
@null:
    lda (r9),Y 
    cmp #0
    bne @diff
@equal:
    lda #0
    rts
@diff:
    lda #1
    rts

.segment "MONITOR"

;; =====================================================================================================
;; main monitor function:
;; =====================================================================================================

monitor:
    ldx #<welcome_msg
    ldy #>welcome_msg
    jsr sys_puts
monitor_loop:          ;; inf loop, global label for exit service
    jsr print_prompt
    jsr get_line
    jsr parse_line

    lda argc            ;; if no input
    cmp #0
    beq monitor_loop

    jsr execute_line  

    jmp monitor_loop

;; =====================================================================================================
;; monitor helper functions:
;; =====================================================================================================

get_line:
    ldx #0
    stx r8
@loop:
    jsr sys_waitchar    ;; echo the input
    cmp #$0D            ;; '\r'
    beq @end  
    cmp #$08            ;; '\b'
    bne @not_bs

    lda r8
    beq @loop           ;; if index == 0, ignore '\b'
    lda #$08            ;; curser back
    jsr sys_putchar
    lda #' '
    jsr sys_putchar
    lda #$08            ;; curser back
    jsr sys_putchar
    dex
    stx r8
    lda #$00
    sta line,x
    jmp @loop
         
@not_bs:
    jsr sys_putchar    
    stx r8              ;; normal char
    sta line,x
    inx
    beq @end            ;; check for line overflow
    jmp @loop
@end:
    lda #$0A            ;; adding new line and null to the line
    jsr sys_putchar
    stx r8
    sta line,x
    inx
    lda #0
    sta line,x  
    rts

parse_line:
    ldx #0             ;; line index
    ldy #0             ;; argv index
    lda #0
    sta argc
@loop:
    jsr trim
    jsr parse_token
    lda line,x
    cmp #$0A           ;; newline? (stop)
    beq @end
    cmp #$00           ;; end of string? (stop)
    beq @end
    jmp @loop
@end:
    rts

trim:
    lda line,x
    cmp #' '
    bne @end
    inx
    jmp trim
@end:
    rts

parse_token:
    lda line,x
    cmp #$0A            ;; newline?
    beq @end_of_line
    cmp #$00            ;; end of string?
    beq @end_of_line
@copy:
    lda line,x
    cmp #' '            ;; space?
    beq @done_token
    cmp #$0A            ;; newline?
    beq @done_token
    cmp #$00            ;; end-of-string?
    beq @done_token

    sta argv,y         
    iny                 
    inx                 
    jmp @copy
@done_token:
    lda #0
    sta argv,y          
    iny                 
    inc argc            
    inx                 
    rts
@end_of_line:
    lda #0
    sta argv,y          
    rts

execute_line:
    ldx #<argv
    ldy #>argv
    stx rdi
    sty rdi+1
    ldx #<mp_dump_str
    ldy #>mp_dump_str
    stx rsi
    sty rsi+1
    jsr sys_strcmp
    cmp #0
    bne @not_dump
    jsr mp_dump
    jmp @end
@not_dump:
    ldx #<argv
    ldy #>argv
    stx rdi
    sty rdi+1
    ldx #<mp_clear_str
    ldy #>mp_clear_str
    stx rsi
    sty rsi+1
    jsr sys_strcmp
    cmp #0
    bne @not_clear
    jsr mp_clear
    jmp @end
@not_clear:
    ldx #<argv
    ldy #>argv
    stx rdi
    sty rdi+1
    ldx #<mp_exit_str
    ldy #>mp_exit_str
    stx rsi
    sty rsi+1
    jsr sys_strcmp
    cmp #0
    bne @not_exit
    jsr mp_exit
    jmp @end
@not_exit:
    ldx #<argv
    ldy #>argv
    stx rdi
    sty rdi+1
    ldx #<mp_run_str
    ldy #>mp_run_str
    stx rsi
    sty rsi+1
    jsr sys_strcmp
    cmp #0
    bne @not_run
    jsr mp_run
    jmp @end
@not_run:

@error:
    lda #ERROR_UNKNOWN_MP
    jsr error
@end:
    rts

;; void error(byte err)
;; input in A
error:
    pha
    ldx #<error_prefix_msg
    ldy #>error_prefix_msg
    jsr sys_puts
    pla
    cmp #ERROR_UNDERFLOW
    beq @underFlow
    cmp #ERROR_UNKNOWN_MP
    beq @unknown_mp
    cmp #ERROR_NO_ARG
    beq @no_arg

@unknown_mp:
    ldx #<error_unknown_mp_msg
    ldy #>error_unknown_mp_msg
    jsr sys_puts
    lda #' '
    jsr sys_putchar
    lda #'"'
    jsr sys_putchar
    ldx #<argv
    ldy #>argv
    jsr sys_puts
    lda #'"'
    jsr sys_putchar
    jmp @end
@underFlow:
    ldx #<error_underFlow_msg
    ldy #>error_underFlow_msg
    jsr sys_puts
    jmp @end
@no_arg:
    ldx #<error_no_arg_msg
    ldy #>error_no_arg_msg
    jsr sys_puts
    jmp @end
@end:
    lda #$0A
    jsr sys_putchar
    rts

print_prompt:
    ldx #<prompt_msg
    ldy #>prompt_msg
    jsr sys_puts
    rts

;; TODO: find a file...
find_file:
    lda #<user_demo
    sta rax
    lda #>user_demo
    sta rax+1
    rts

hello_msg: .byte "Hello User!", $0A, 0

user_demo:
    ldx #<hello_msg
    ldy #>hello_msg
    jsr sys_puts
    rts

;; =====================================================================================================
;; monitor sub programs:
;; =====================================================================================================

mp_dump:

    ldy #0              
    ldx #0              
@next_arg:
    cpx argc            
    beq @done 
@print_arg:
    lda argv,y 
    beq @end_of_arg      
    jsr sys_putchar      
    iny                 
    jmp @print_arg
@end_of_arg:
    lda #$0A
    jsr sys_putchar
    lda #$0D
    jsr sys_putchar   
    iny                 
    inx                 
    jmp @next_arg
@done:
    rts

mp_run:
    lda argc
    cmp #1
    beq @no_arg
    lda #<argv
    clc
    adc #4       ;; len("run") == 3
    sta rdi
    lda #>argv
    sta rdi+1
    jsr find_file
    jmp (rax)
    rts
@no_arg:
    lda #ERROR_NO_ARG
    jsr error
    rts

mp_clear:
    jsr sys_clear
    rts

mp_exit:
    jmp reset

;; =====================================================================================================
;; called during RESET:
;; =====================================================================================================

.segment "STARTUP"
reset:
    ;; init the system memory
    ;; TODO: consider more memory init's
    cld
    sei
    ldx #$FF
    txs
    lda #STACK_START_H
    sta rsp+1
    lda #STACK_START_L
    sta rsp

    ;; install the built-in NMI/IRQ/BRK handlers
    ldx #<nmi_handler
    ldy #>nmi_handler
    lda #$4C                    ;; jmp absolute
    sta nmi_hook+0
    stx nmi_hook+1
    sty nmi_hook+2

    ldx #<irq_handler
    ldy #>irq_handler
    sta irq_hook+0
    stx irq_hook+1
    sty irq_hook+2

    cli                         ;; from here, start of interactive part

    jsr sys_clear  

    jmp monitor                 ;; inf loop

    brk

;; ====================================================================================================
;; built-in interrupt handlers (the handlers address are loaded to RAM in startup, user can modify): 
;; ====================================================================================================

nmi_handler:
    pha
    txa
    pha
    tya
    pha
    ;; TODO: do staff... NMI power-off the cpu for now...
    lda #POWER_OFF
    sta power
    pla
    tay
    pla
    tax
    pla
    rti

irq_handler:
    pha
    txa
    pha
    tya
    pha

    tsx            
    lda $0104,x
    and #$10       ;; test bit 4 (B flag)
    bne brk_handler     

    ;; TODO: do staff for a hardware irq...

irq_end:
    pla
    tay
    pla
    tax
    pla
    rti

brk_handler:
    ;; TODO: do staff for a software interrupts...
    jmp irq_end

;; =====================================================================================================
;; jmp table for kernel services (syscalls), for a uniform user interface:
;; =====================================================================================================

.segment "DISPATCH"

putchar:    jmp sys_putchar
clear:      jmp sys_clear  
waitchar:   jmp sys_waitchar
puts:       jmp sys_puts
strcmp:     jmp sys_strcmp

brk

;; =====================================================================================================
;; interrupt and RESET vectors:
;; =====================================================================================================
    
.segment "VECTORS"
.word nmi_hook
.word reset
.word irq_hook
