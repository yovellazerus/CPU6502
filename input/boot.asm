
.include "arch0.inc"

.segment "BOOT"
.org boot_entry
_start:
    
    ldx #<hello_msg     ;; hello msg
    ldy #>hello_msg
    jsr puts

    ldx $FF             ;; copy the buffer to the disk data
@write_buffer:
    lda buffer,x
    sta DISK_DATA,x
    dex
    bne @write_buffer

    lda #$00            ;; write the buffer to $0001 block of the disk, not working...
    sta DISK_ADDRH
    lda #$01
    sta DISK_ADDRL
    lda #DISK_WRITE
    sta DISK_CMD
@wait_disk:
    lda DISK_STATUS
    cmp #DISK_READY
    bne @wait_disk

    lda #POWER_OFF      ;; power-off the CPU
    sta POWER

;; void puts(X: strL, Y: strH)
puts:
    stx STR       
    sty STR+1        
    ldy #0
@loop:
    lda (STR),y
    beq @done   
    jsr putchar
    iny             
    bne @loop  
    inc STR+1        
    jmp @loop
@done:
    rts

;; void putchar(A : char)
putchar:
    sta SCR_DATA     
    lda #SCR_WRITE
    sta SCR_CTRL
    rts
    
hello_msg: .byte "Hello BOOT!", $0A, 0
buffer:    .byte "I wrote this to disk!", $0A, 0
