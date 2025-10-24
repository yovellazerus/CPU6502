
;*******************************************************************************
;*******          ROM BASE OS — Core Firmware for the 6502           ***********
;*******************************************************************************

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

;; ================================================================================
;; macros:
;; ================================================================================

; copy 8 bytes from src to dest
.macro MOV64 src, dest
    ldx #0
@loop:
    lda src,x
    sta dest,x
    inx
    cpx #8
    bne @loop
.endmacro


;; ================================================================================
;; consts:
;; ================================================================================

;; KERNEL constants

STACK_START_H    = $0F
STACK_START_L    = $FF
ERROR_UNDERFLOW  = $01
ERROR_UNKNOWN_MP = $02

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

rax = ZP_X86_64 + $00
rbx = ZP_X86_64 + $08
rcx = ZP_X86_64 + $10
rdx = ZP_X86_64 + $18
rsi = ZP_X86_64 + $20
rdi = ZP_X86_64 + $28
rbp = ZP_X86_64 + $30
rsp = ZP_X86_64 + $38
r8  = ZP_X86_64 + $40
r9  = ZP_X86_64 + $48
r10 = ZP_X86_64 + $50
r11 = ZP_X86_64 + $58
r12 = ZP_X86_64 + $60
r13 = ZP_X86_64 + $68
r14 = ZP_X86_64 + $70
r15 = ZP_X86_64 + $78

;; ================================================================================
;; KERNEL zero page vars:
;; ================================================================================

KERNEL_ZP    = $80

line_size    = KERNEL_ZP + $42
argc         = KERNEL_ZP + $50

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

;; ================================================================================
;; kernel working RAM
;; ================================================================================

KERNEL_RAM   = $C200

line         = KERNEL_RAM + $0000
argv         = KERNEL_RAM + $0100

;; ================================================================================
;;; kernel code and read only data is here
;; ================================================================================

.segment "RODATA"
welcome_msg:          .byte "**** 6502 kernel monitor ****", $0A, 0
prompt_msg:           .byte "> ", 0

error_prefix_msg:     .byte "ERROR: ", 0
error_underFlow_msg:  .byte "underFlow", 0
error_unknown_MP_msg: .byte "unknown monitor service", 0

MP_dump_str:          .byte "dump", 0
MP_clear_str:         .byte "clear", 0

.segment "LIB"
;; void string_print(str s) the arg "s" is in X,Y
string_print:
    stx r8       
    sty r8+1        
    ldy #0
print_loop:
    lda (r8),y
    beq print_done   
    jsr bios_putchar
    iny             
    bne print_loop  
    inc r8+1        
    jmp print_loop
print_done:
    rts

;; A : bool string_cmp(str a, str b)
;; note: 0 if eq 1 if diff no <,> for now
string_cmp:
    
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

;; void error(A : err) "err" in A
error:
    ldx #<error_prefix_msg
    ldy #>error_prefix_msg
    jsr string_print
    cmp #ERROR_UNDERFLOW
    beq @underFlow
    cmp #ERROR_UNKNOWN_MP
    beq @unknown_MP

@unknown_MP:
    ldx #<error_unknown_MP_msg
    ldy #>error_unknown_MP_msg
    jsr string_print
    lda #' '
    jsr bios_putchar
    lda #'"'
    jsr bios_putchar
    ldx #<argv
    ldy #>argv
    jsr string_print
    lda #'"'
    jsr bios_putchar
    jmp @end
@underFlow:
    ldx #<error_underFlow_msg
    ldy #>error_underFlow_msg
    jsr string_print
    jmp @end
@end:
    lda #$0A
    jsr bios_putchar
    rts

.segment "MONITOR"
monitor:
    ldx #<welcome_msg
    ldy #>welcome_msg
    jsr string_print
@loop:                  ;; inf loop
    jsr print_prompt
    jsr get_line
    jsr parse_line

    lda argc            ;; if no input
    cmp #0
    beq @loop 

    jsr execute_line  

    jmp @loop

;; 
get_line:
    ldx #0
    stx line_size
@loop:
    jsr bios_waitchar    ;; echo the input
    jsr bios_putchar
    cmp #$0D            ;; is 'cr'?
    beq @end           ;; yes
    stx line_size      ;; no, next char
    sta line,x
    inx
    jmp @loop
@end:
    lda #$0A            ;; adding new line and null to the line
    jsr bios_putchar
    stx line_size
    sta line,x
    inx
    lda #0
    sta line,x  
    rts

parse_line:
    ldx #0             ; line index
    ldy #0             ; argv index
    lda #0
    sta argc
@loop:
    jsr trim
    jsr parse_token
    lda line,x
    cmp #$0A           ; newline? (stop)
    beq @end
    cmp #$00           ; end of string? (stop)
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
    cmp #$0A            ; newline?
    beq @end_of_line
    cmp #$00            ; end of string?
    beq @end_of_line
@copy:
    lda line,x
    cmp #' '            ; space?
    beq @done_token
    cmp #$0A            ; newline?
    beq @done_token
    cmp #$00            ; end-of-string?
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
    ldx #<MP_dump_str
    ldy #>MP_dump_str
    stx rsi
    sty rsi+1
    jsr string_cmp
    cmp #0
    bne @not_dump
    jsr MP_dump
    jmp @end
@not_dump:
    ldx #<argv
    ldy #>argv
    stx rdi
    sty rdi+1
    ldx #<MP_clear_str
    ldy #>MP_clear_str
    stx rsi
    sty rsi+1
    jsr string_cmp
    cmp #0
    bne @not_clear
    jsr MP_clear
    jmp @end
@not_clear:

@error:
    lda #ERROR_UNKNOWN_MP
    jsr error
@end:
    rts

print_prompt:
    ldx #<prompt_msg
    ldy #>prompt_msg
    jsr string_print
    rts

MP_dump:
    ldy #0              
    ldx #0              
@next_arg:
    cpx argc            
    beq @done            
@print_arg:
    lda argv,y          
    beq @end_of_arg      
    jsr bios_putchar      
    iny                 
    jmp @print_arg
@end_of_arg:
    lda #$0A
    jsr bios_putchar
    lda #$0D
    jsr bios_putchar   
    iny                 
    inx                 
    jmp @next_arg
@done:
    rts

MP_clear:
    jsr bios_clear
    rts

.segment "BIOS"
reset:
    ldx #$FF                    ;; BIOS init's 
    txs
    cld
    cli
    lda #STACK_START_H
    sta rsp+1
    lda #STACK_START_L
    sta rsp
    jsr bios_clear

    MOV64 welcome_msg, $B000    ;; macro test, macros are amazing!!!!

    jmp monitor                 ;; not returning, inf loop

bios_nmi_handler:
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

bios_irq_handler:
    pha
    txa
    pha
    tya
    pha

    tsx            
    lda $0104,x
    and #$10       ;; test bit 4 (B flag)
    beq bios_irq_is_irq     

    jsr bios_brk_handler
    jmp bios_irq_end

bios_irq_is_irq:
    ;; TODO: do staff...

bios_irq_end:
    pla
    tay
    pla
    tax
    pla
    rti

bios_brk_handler:
    ;; TODO: do staff...
    rts

;; void putchar(A : char)
bios_putchar:
    pha
    sta scr_data     
    lda #SCR_WRITE
    sta scr_ctrl
    pla
    rts

;; void clear(void)
bios_clear:
    pha
    lda #SCR_CLEAR
    sta scr_ctrl
    pla
    rts

;; A : char waitchar()
bios_waitchar:
    lda keb_ctrl
    cmp #KEB_READY
    bne bios_waitchar 
    lda #0
    sta keb_ctrl
    lda keb_data
    rts

.segment "VECTORS"
.word bios_nmi_handler
.word reset
.word bios_irq_handler
