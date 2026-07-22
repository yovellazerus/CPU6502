
.segment "TRAMPOLINE"

.importzp ptr1
.importzp ptr2

.import _syscall_entry
.import _kernel_irq
.import _kernel_nmi

MMU_PAGE_TABLE = $fe20 ;; 16 bytes

.global _irq_handler
_irq_handler:
    sta _life_raft + 6
    sty _life_raft + 5
    stx _life_raft + 4
    pla
    sta _life_raft + 3
    pla
    sta _life_raft + 2
    pla
    sta _life_raft + 1
    tsx
    stx _life_raft + 0 

    lda $0103, x
    and #$10
    beq @irq
    jmp _syscall_entry ;; brk
@irq:
    jmp _kernel_irq
    

.global _nmi_handler
_nmi_handler:
    sta _life_raft + 6
    sty _life_raft + 5
    stx _life_raft + 4
    pla
    sta _life_raft + 3
    pla
    sta _life_raft + 2
    pla
    sta _life_raft + 1
    tsx
    stx _life_raft + 0 
    jmp _kernel_nmi

; Assembly routine that installs the page table, restores context, and executes RTI
; void return_from_trap(uint8_t* page_table);
.global _return_from_trap
_return_from_trap:  
    sta ptr1+0
    stx ptr1+1

    ldy #$00          
@mmu_loop:
    lda (ptr1), y     
    sta MMU_PAGE_TABLE, y      
    iny
    cpy #$0F          ; Have we done segments 0 through 14?
    bne @mmu_loop

    ldx _life_raft + 0         ; Restore user Stack Pointer
    txs               
    
    lda _life_raft + 1         ; Push Status Register for RTI
    pha
    lda _life_raft + 2         ; Push PC High for RTI
    pha
    lda _life_raft + 3         ; Push PC Low for RTI
    pha

    ldx _life_raft + 4         ; Restore X register
    ldy _life_raft + 5         ; Restore Y register
    lda _life_raft + 6         ; Restore A register last

    sta $F000         ; TODO: not implemented
    rti               

.global _life_raft
_life_raft:
    .res 8