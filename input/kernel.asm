
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


;;; FIX-RAM:

;; zero page:
ZP           = $00

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
MSRC          = ZP + $46 ;; for _mov(to not use TMP)
MDST          = ZP + $48   

;; I/O:
KED_DATA     = $0210
KED_CTRL     = $0211
DIS_DATA     = $0212
DIS_CTRL     = $0213

;; disk I/O buffer
DISK         = $0300

;; kernel working RAM:
LINE         = $0400
ROOT         = $0500
PCB          = $0600
EXTRA        = $0700

;; software stack:
KERNEL_RAM  = $0800
SS_LOW      = $0C00
SS_HIGH     = $0FFF    

;;; ROM:

.segment "RODATA"
welcome_string: .byte "6","5","0","2","-","m","o","n","i","t","o","r",":", $0A, 0

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

;; void _mov(src: PTR0, dst: PTR1, size: A) size < 256 for  now!
_mov:
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

.segment "MONITOR"
monitor:
    ldy #0              ;; init vars
    ldx #0
    stx LINE_INDEX
    jsr print_prompt
@get_line:
    jsr bios_getchar    ;; echo the input
    jsr bios_putchar
    cmp #$0D            ;; is 'cr'?
    beq @new_line       ;; yes
    stx LINE_INDEX      ;; no, next char
    sta LINE,x
    inx
    jmp @get_line
@new_line:
    lda #$0A            ;; adding new line and null to the LINE
    jsr bios_putchar
    stx LINE_INDEX
    sta LINE,x
    inx
    lda #0
    sta LINE,x           
    ldx #<LINE          ;; echo the LINE to the user(for now...)
    ldy #>LINE
    jsr string_print
    jmp monitor

print_prompt:
    pha
    lda #'>'
    jsr bios_putchar
    lda #' '
    jsr bios_putchar
    pla
    rts

.segment "BIOS"
RESET:
    ldx #$FF
    txs
    cld
    cli

    ldx #<welcome_string    ;; mov test (not in ZP and longer then size == 4)
    stx PTR0
    lda #>welcome_string
    sta PTR0+1

    ldy #<DISK
    sty PTR1
    lda #>DISK
    sta PTR1+1

    lda #16
    jsr _mov

    brk             ;; for brk test
    .byte $42      


    ldx #<welcome_string
    ldy #>welcome_string
    jsr string_print
    jmp monitor    ;; inf loop

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
    sta DIS_DATA     
    lda #1
    sta DIS_CTRL
    pla
    rts

;; A : char getchar()
bios_getchar:
    lda KED_CTRL   ;; wait for a user input
    cmp #1
    bne bios_getchar 
    lda #0
    sta KED_CTRL
    lda KED_DATA
    rts

.byte $FF           ;; stop the cpu for debug

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
