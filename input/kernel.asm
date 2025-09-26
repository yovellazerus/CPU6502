
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
ERROR_UNDERFLOW = $01


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
LINE_INDEX   = ZP + $42
_A           = ZP + $43   ;; tmp's to save the cpu registers 
_X           = ZP + $44
_Y           = ZP + $45
MSRC         = ZP + $46   ;; for mov(to not use TMP)
MDST         = ZP + $48   

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
ROOT         = $0500
PCB          = $0600
KERNEL_RAM   = $0700

;; software stack:
SS_LOW  = $0800
SS_HIGH = $0FFF    

;;; ROM:

.segment "RODATA"
welcome_msg:         .byte $0A, "**** 6502 ROM computer monitor for all of mankind ****", $0A, 0
prompt_msg:          .byte "> ", 0
error_prefix_msg:    .byte "ERROR: ", 0
error_underFlow_msg: .byte "underFlow", 0

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

@underFlow:
    ldx #<error_underFlow_msg
    ldy #>error_underFlow_msg
    jsr string_print
    jmp @end
@end:
    lda #$0A
    jsr bios_putchar
    .byte $FF

.segment "MONITOR"
monitor:
    ldx #<welcome_msg
    ldy #>welcome_msg
    jsr string_print
@loop:                  ;; inf loop
    jsr print_prompt
    jsr get_line

    lda LINE            ;; if no input
    cmp #$0A
    beq @loop   

    ldx #<LINE          ;; echo the LINE to the user(for now...)
    ldy #>LINE
    jsr string_print
    jmp @loop

get_line:
    PROLOGUE
    ldx #0
    stx LINE_INDEX
@loop:
    jsr bios_waitchar    ;; echo the input
    jsr bios_putchar
    cmp #$0D            ;; is 'cr'?
    beq @end           ;; yes
    stx LINE_INDEX      ;; no, next char
    sta LINE,x
    inx
    jmp @loop
@end:
    lda #$0A            ;; adding new line and null to the LINE
    jsr bios_putchar
    stx LINE_INDEX
    sta LINE,x
    inx
    lda #0
    sta LINE,x  
    EPILOGUE
    rts

print_prompt:
    PROLOGUE
    ldx #<prompt_msg
    ldy #>prompt_msg
    jsr string_print
    EPILOGUE
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

    jmp monitor

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
