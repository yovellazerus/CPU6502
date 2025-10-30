
;********************************************************************************************************
;*************               ROM BASE OS - Core Firmware for the 6502           *************************
;********************************************************************************************************

.include "arch0.inc"

;; =====================================================================================================
;;; kernel read only data is here:
;; =====================================================================================================

.segment "RODATA"

welcome_msg:                     .byte "**** welcome to 6502 kernel monitor ****", $0A, 0
prompt_msg:                      .byte "> ", 0
         
error_prefix_msg:                .byte "ERROR: ", 0
error_underFlow_msg:             .byte "underFlow", 0
error_unknown_mp_msg:            .byte "unknown monitor service", 0
error_no_arg_msg:                .byte "no arguments were provided", 0
error_no_user_program_msg:       .byte "no user program: ", 0

mp_dump_str:                     .byte "dump", 0
mp_clear_str:                    .byte "clear", 0

.segment "LIB"

;; void sys_putchar(char c)
;; input in A
sys_putchar:
    pha
    sta mmio_scr_data     
    lda #SCR_WRITE
    sta mmio_scr_ctrl
    pla
    rts

;; void sys_clear(void)
sys_clear:
    pha
    lda #SCR_CLEAR
    sta mmio_scr_ctrl
    pla
    rts

;; char sys_waitchar()
;; output in A
sys_waitchar:
    lda mmio_keb_ctrl
    cmp #KEB_READY
    bne sys_waitchar 
    lda #0
    sta mmio_keb_ctrl
    lda mmio_keb_data
    rts

;; void sys_puts(char* s) 
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

;; void sys_load_sector(u16 sector_id)
;; read disk sector to disk_buffer
;; input in X, Y
sys_load_sector:
    stx mmio_disk_addrl
    sty mmio_disk_addrh
    lda #DISK_READ
    sta mmio_disk_cmd
@wait_disk:
    lda mmio_disk_status
    cmp #DISK_READY
    bne @wait_disk

    MOV mmio_disk_data, disk_buffer, #$FF

    rts

;; bool sys_strcmp(str a, str b)
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

;; void sys_exit(unsigned char code)
;; end user "proses" do not return
sys_exit:
    ldx #$FF
    txs
    lda #STACK_START_H
    sta rsp+1
    lda #STACK_START_L
    sta rsp
    jmp monitor

;; void sys_execute(char* path)
;; start user "proses", user program return to this function 
sys_execute:
    jsr sys_open
    bne @no_file
    MOV file_buffer, user_entry, #$FF
    jmp (rax)
    jsr sys_exit
@no_file:
    lda #NO_USER_PROGRAM
    rts

;; void* sys_open(char* path), status is return in A
;; open a file, file start address is in rax
sys_open:
    ldx #01     ;; loading $0001 sector, were the root dir is for now
    ldy #00
    jsr sys_load_sector

    MOV disk_buffer, root_dir, #$FF

    ldx #<root_dir
    ldy #>root_dir
    stx rcx+0
    sty rcx+1

    stx rdi+0
    sty rdi+1
    ldx #<argv
    ldy #>argv
    stx rsi+0
    sty rsi+1
    jsr sys_strcmp
    beq @found
@loop:
    lda rcx
    clc
    adc #32        ;; dir entry size: file name 16B, sector number 2B, 14B unused
    beq @not_found
    sta rcx

    ldx rcx+0
    ldy rcx+1
    stx rdi+0
    sty rdi+1
    ldx #<argv
    ldy #>argv
    stx rsi+0
    sty rsi+1
    jsr sys_strcmp
    bne @loop

@found:
    ldy #16
    lda (rcx),y
    tax
    ldy #17
    lda (rcx),y
    tay
    jsr sys_load_sector
    
    ldx #$FF
@copy:
    lda disk_buffer,x
    sta file_buffer,x
    dex
    bne @copy

    lda #<file_buffer
    sta rax+0
    lda #>file_buffer
    sta rax+1
    lda #0   ;; ok
    rts
@not_found:
    lda #NO_FILE_FOUND
    rts
    

.segment "MONITOR"

;; =====================================================================================================
;; main monitor function:
;; =====================================================================================================

monitor:
    jsr print_prompt
    jsr get_line
    jsr parse_line

    lda argc            ;; if no input
    beq monitor

    jsr monitor_program
    beq monitor

    lda #<argv
    sta rdi+0
    lda #>argv
    sta rdi+1
    jsr sys_execute 
    beq monitor

    jsr error
    lda #'"'
    jsr sys_putchar
    ldx #<argv
    ldy #>argv
    jsr sys_puts
    jsr sys_putchar

    jmp monitor

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

monitor_program:
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

@error:
    lda #ERROR_UNKNOWN_MP
    rts
@end:
    rts

;; void error(char err)
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
@no_user_program:
    ldx #<error_no_user_program_msg
    ldy #>error_no_user_program_msg
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

mp_clear:
    jsr sys_clear
    rts

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

    jsr sys_clear
    ldx #<welcome_msg
    ldy #>welcome_msg
    jsr sys_puts

    cli                         ;; from here, start of interactive part

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
    ;; TODO: do staff... NMI mmio_power-off the cpu for now...
    lda #POWER_OFF
    sta mmio_power
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

putchar:        jmp sys_putchar
clear:          jmp sys_clear  
waitchar:       jmp sys_waitchar
puts:           jmp sys_puts
strcmp:         jmp sys_strcmp
load_sector:    jmp sys_load_sector
exit:           jmp sys_exit
execute:        jmp sys_execute
open:           jmp sys_open

brk

;; =====================================================================================================
;; interrupt and RESET vectors:
;; =====================================================================================================
    
.segment "VECTORS"
.word nmi_hook
.word reset
.word irq_hook
