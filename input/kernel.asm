
;;: MACROS:

.macro PROLOGUE
    pha
    txa
    pha
    tya
    pha
.endmacro

.macro EPILOGUE
    pla
    tay
    pla
    tax
    pla
.endmacro

ZP  = $00
SSPH_START = $0F
SSPL_START = $FF
SSPH_END = $08
SSPL_END = $00
ERROR_UNDERFLOW  = $01
ERROR_UNKNOWN_MP = $02


;;; FIX-RAM:

;; zero page:

ARG0         = ZP + $00
ARG1         = ZP + $04
ARG2         = ZP + $08
ARG3         = ZP + $0C

TMP0         = ZP + $10
TMP1         = ZP + $14
TMP2         = ZP + $18
TMP3         = ZP + $1C

RET0         = ZP + $20
RET1         = ZP + $24
RET2         = ZP + $28
RET3         = ZP + $2C

PTR0         = ZP + $30
PTR1         = ZP + $32

FPA0         = ZP + $34
FPA1         = ZP + $38

SSP          = ZP + $3C
SBP          = ZP + $3E

ERR          = ZP + $40
NMIC         = ZP + $41
LINE_SIZE   = ZP + $42
_A           = ZP + $43   ;; tmp's to save the cpu registers 
_X           = ZP + $44
_Y           = ZP + $45
MSRC         = ZP + $46   ;; for mov(to not use TMP)
MDST         = ZP + $48  
ARGC         = ZP + $50

;; I/O:
KEB_DATA     = $0210
KEB_CTRL     = $0211
SCR_DATA     = $0212
SCR_CTRL     = $0213

DISK_CTRL    = $0214
DISK_STAT    = $0215
UDI_DATA     = $0216
UDI_CTRL     = $0217
RAND         = $0218
TIMER_DATAL  = $0219
TIMER_DATAH  = $021A
TIMER_LACHL  = $021B
TIMER_LACHH  = $021C
TIMER_CTRL   = $021D

;; disk I/O buffer
DISK         = $0300

;; kernel working RAM:
LINE         = $0400
ARGV         = $0500
ROOT         = $0600
PCB          = $0700

;; software stack:
SS_LOW  = $0800
SS_HIGH = $0FFF    

;;; ROM:

.segment "RODATA"
welcome_msg:          .byte $0A, "**** 6502 kernel monitor for all of mankind ****", $0A, 0
prompt_msg:           .byte "> ", 0

error_prefix_msg:     .byte "ERROR: ", 0
error_underFlow_msg:  .byte "underFlow", 0
error_unknown_MP_msg: .byte "unknown monitor service", 0

MP_dump_str:          .byte "dump", 0

.segment "LIB"
;; void string_print(X: PTR0L, Y: PTR0H)
string_print:
    PROLOGUE
    stx PTR0       
    sty PTR0+1        
    ldy #0
print_loop:
    lda (PTR0),y
    beq print_done   
    jsr bios_putchar
    iny             
    bne print_loop  
    inc PTR0+1        
    jmp print_loop
print_done:
    EPILOGUE
    rts

;; A : bool string_cmp(PTR0, PTR1)
;; note: 0 if eq 1 if diff no <,> for now
string_cmp:
    
    lda PTR0
    sta TMP0
    lda PTR0+1
    sta TMP0+1

    lda PTR1
    sta TMP1
    lda PTR1+1
    sta TMP1+1

@loop:
    ldy #0
    lda (TMP0),Y
    cmp #0
    beq @null       
    sta TMP2           
    lda (TMP1),Y     
    cmp TMP2       
    bne @diff                   
    inc TMP0
    bne @skip0
    inc TMP0+1
@skip0:
    inc TMP1
    bne @skip1
    inc TMP1+1
@skip1:
    jmp @loop
@null:
    lda (TMP1),Y 
    cmp #0
    bne @diff
@equal:
    lda #0
    rts
@diff:
    lda #1
    rts


;; void mov(src: PTR0, dst: PTR1, size: A) size < 256 for  now!
mov:
    sta _A            
    PROLOGUE
    lda PTR0        
    sta MSRC
    lda PTR0+1      
    sta MSRC+1
    lda PTR1        
    sta MDST
    lda PTR1+1      
    sta MDST+1
    ldy #0             
@loop:
    lda (MSRC),y    
    sta (MDST),y    
    iny
    cpy _A
    bne @loop
    EPILOGUE
    rts
@done:
    EPILOGUE
    rts

;; void error(A : err) power-off the cpu(for debug)
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
    ldx #<ARGV
    ldy #>ARGV
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

    lda ARGC            ;; if no input
    cmp #0
    beq @loop 

    jsr execute_line  

    jmp @loop

get_line:
    PROLOGUE
    ldx #0
    stx LINE_SIZE
@loop:
    jsr bios_waitchar    ;; echo the input
    jsr bios_putchar
    cmp #$0D            ;; is 'cr'?
    beq @end           ;; yes
    stx LINE_SIZE      ;; no, next char
    sta LINE,x
    inx
    jmp @loop
@end:
    lda #$0A            ;; adding new line and null to the LINE
    jsr bios_putchar
    stx LINE_SIZE
    sta LINE,x
    inx
    lda #0
    sta LINE,x  
    EPILOGUE
    rts

parse_line:
    ldx #0             ; LINE index
    ldy #0             ; ARGV index
    lda #0
    sta ARGC
@loop:
    jsr trim
    jsr parse_token
    lda LINE,x
    cmp #$0A           ; newline? (stop)
    beq @end
    cmp #$00           ; end of string? (stop)
    beq @end
    jmp @loop
@end:
    rts

trim:
    lda LINE,x
    cmp #' '
    bne @end
    inx
    jmp trim
@end:
    rts

parse_token:
    lda LINE,x
    cmp #$0A            ; newline?
    beq @end_of_line
    cmp #$00            ; end of string?
    beq @end_of_line
@copy:
    lda LINE,x
    cmp #' '            ; space?
    beq @done_token
    cmp #$0A            ; newline?
    beq @done_token
    cmp #$00            ; end-of-string?
    beq @done_token

    sta ARGV,y         
    iny                 
    inx                 
    jmp @copy
@done_token:
    lda #0
    sta ARGV,y          
    iny                 
    inc ARGC            
    inx                 
    rts
@end_of_line:
    lda #0
    sta ARGV,y          
    rts

execute_line:
    ldx #<ARGV
    ldy #>ARGV
    stx PTR0
    sty PTR0+1
    ldx #<MP_dump_str
    ldy #>MP_dump_str
    stx PTR1
    sty PTR1+1
    jsr string_cmp
    cmp #0
    bne @not_dump
    jsr MP_dump
    jmp @end
@not_dump:

@error:
    lda #ERROR_UNKNOWN_MP
    jsr error
@end:
    rts

print_prompt:
    PROLOGUE
    ldx #<prompt_msg
    ldy #>prompt_msg
    jsr string_print
    EPILOGUE
    rts

MP_dump:
    ldy #0              
    ldx #0              
@next_arg:
    cpx ARGC            
    beq @done            
@print_arg:
    lda ARGV,y          
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

.segment "BIOS"
RESET:
    ldx #$FF                ;; BIOS init's 
    txs
    cld
    cli
    lda #SSPH_START
    sta SSP+1
    lda #SSPL_START
    sta SSP
    jsr bios_clear

    ldx #<welcome_msg    ;; mov test (not in ZP and longer then size == 4)
    stx PTR0
    lda #>welcome_msg
    sta PTR0+1

    ldy #<DISK
    sty PTR1
    lda #>DISK
    sta PTR1+1

    lda #16
    jsr mov

    brk             ;; for brk test
    .byte $42      

    jmp monitor     ;; not returning, inf loop

nmi_handler:
    PROLOGUE
    ldx NMIC
    inx
    stx NMIC
    EPILOGUE
    rti

irq_handler:
    PROLOGUE
    jmp brk_handler
    ;; no real irq, for now only brk
irq_end:
    EPILOGUE
    rti

brk_handler:
    tsx
    inx
    inx
    inx
    inx
    inx
    lda $0100,x
    tax
    dex
    lda RESET,x     ;; PCH is coded at RESET for now
    sta ERR
    jmp irq_end

;; void putchar(A : char)
bios_putchar:
    pha
    sta SCR_DATA     
    lda #1
    sta SCR_CTRL
    pla
    rts

;; void clear(void)
bios_clear:
    pha
    lda #2
    sta SCR_CTRL
    pla
    rts

;; A : char waitchar()
bios_waitchar:
    lda KEB_CTRL   ;; wait for a user input
    cmp #1
    bne bios_waitchar 
    lda #0
    sta KEB_CTRL
    lda KEB_DATA
    rts

.byte $FF           ;; stop the cpu for debug

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
