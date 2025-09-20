
;; zero page:
ARG0         = $00
ARG1         = $01
tmp          = $02
brk_byte     = $03
nmi_counter  = $04
cmd_index    = $05
PTR0         = $06
PTR1         = $07


;; I/O:
keyboard_data   = $D010
keyboard_ctrl   = $D011
screen_data     = $D012
screen_ctrl     = $D013

;; monitor vars:
cmd        = $0300

.segment "CODE"
RESET:
    lda #$11
    sta ARG0
    lda #$22
    brk         ; test for brk puts the byte in brk_byte in zero-page
    .byte $42 
    sta ARG1
    jsr swap

    ldx #<welcome_string
    ldy #>welcome_string
    jsr string_print

    jsr monitor ; inf loop

monitor:
    ldy #0              ;; init vars
    ldx #0
    stx cmd_index
    jsr print_prompt
@get_line:
    jsr bios_getchar    ;; echo the input
    jsr bios_putchar
    cmp #$0D            ;; is 'cr'?
    beq @new_line       ;; yes
    stx cmd_index       ;; no, next char
    sta cmd,x
    inx
    jmp @get_line
@new_line:
    lda #$0A            ;; adding new line and null to the cmd
    jsr bios_putchar
    stx cmd_index
    sta cmd,x
    inx
    lda #0
    sta cmd,x           
    ldx #<cmd           ;; echo the cmd to the user(for now...)
    ldy #>cmd
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

swap:
    pha         ; prologue
    lda ARG0
    sta tmp    ; byte tmp = a
    lda ARG1
    brk
    .byte $33   ; test for brk puts the byte in brk_byte in zero-page
    sta ARG0    ; a = b
    lda tmp
    sta ARG1    ; b = tmp
    pla         ; epilogue
    rts

nmi_handler:
    pha         ; prologue
    txa
    pha
    tya
    pha

    ldx nmi_counter
    inx
    stx nmi_counter
    
    pla         ; epilogue
    tay
    pla
    tax
    pla
    rti

irq_handler:
    pha         ; prologue
    txa
    pha
    tya
    pha

    jmp brk_handler
    ; no real irq, for now only brk

irq_end:
    pla         ; epilogue
    tay
    pla
    tax
    pla
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
    lda RESET,x ; PCH is coded at RESET for now
    sta brk_byte
    jmp irq_end

; void putchar(A : char)
bios_putchar:
    pha
    sta screen_data     
    lda #1
    sta screen_ctrl
    pla
    rts

; A : char getchar()
bios_getchar:
    lda keyboard_ctrl
    cmp #1
    bne bios_getchar 

    lda #0
    sta keyboard_ctrl
    lda keyboard_data
    rts

; void string_print(X: ptrL, Y: ptrH)
string_print:
    pha
    txa
    pha
    tya
    pha

    stx PTR0       
    sty PTR1        
    ldy #0

print_loop:
    lda (PTR0),y
    beq print_done   
    jsr bios_putchar
    iny             
    bne print_loop  
    inc PTR1        
    jmp print_loop

print_done:
    pla
    tay
    pla
    tax
    pla
    rts

welcome_string: .byte "6","5","0","2","-","m","o","n","i","t","o","r",":", $0A, 0

.byte $FF       ; stop the cpu for debug

.segment "VECTORS"
.word nmi_handler
.word RESET
.word irq_handler
