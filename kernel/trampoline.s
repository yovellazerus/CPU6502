
.segment "TRAMPOLINE"

.importzp ptr1
.importzp ptr2

.import _kernel_brk
.import _kernel_irq
.import _kernel_nmi

MMU_PAGE_TABLE = $fe20 ;; 16 bytes
USER_RTI       = $0100 ;; close interrupt and resume user code, running in user space to close seg15

.global _irq_handler
_irq_handler:
    ;; save user CPU registers to the life raft
    sta user_context + 6
    sty user_context + 5
    stx user_context + 4
    pla
    sta user_context + 1   ;; P (Status) is always pulled first
    pla
    sta user_context + 2   ;; PCL (PC Low) is pulled second
    pla
    sta user_context + 3   ;; PCH (PC High) is pulled last
    tsx
    stx user_context + 0

    ;; switch the memory map to kernel space, 
    ;; all of them except for seg15, 
    ;; because we are currently running code on it
    ldx #$00          
@mmu_loop:
    lda kernel_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ; have we done segments 0 through 14?
    bne @mmu_loop
    
    ;; jmp to C functions in the kernel
    lda user_context + 1   ;; Load Status (P)
    and #%00010000         ;; Check the B flag
    beq @irq
    jmp _kernel_brk
@irq:
    jmp _kernel_irq
    

.global _nmi_handler
_nmi_handler:
    ;; save user CPU registers to the life raft
    sta user_context + 6
    sty user_context + 5
    stx user_context + 4
    pla
    sta user_context + 1   ;; P (Status) is always pulled first
    pla
    sta user_context + 2   ;; PCL (PC Low) is pulled second
    pla
    sta user_context + 3   ;; PCH (PC High) is pulled last
    tsx
    stx user_context + 0

    ;; switch the memory map to kernel space, 
    ;; all of them except for seg15, 
    ;; because we are currently running code on it
    ldx #$00          
@mmu_loop:
    lda kernel_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ; have we done segments 0 through 14?
    bne @mmu_loop
    
    ;; jmp to C function in the kernel
    jmp _kernel_nmi

; Assembly routine that installs the page table, restores context, install return to user stub
; void return_from_trap(void);
.global _return_from_trap
_return_from_trap:  

    ;; restore user memory space form life raft
    ldx #$00          
@mmu_loop:
    lda user_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ; Have we done segments 0 through 14?
    bne @mmu_loop

    ;; restore CPU registers form life raft
    ldx user_context + 0         ;; SP
    txs               
    lda user_context + 3         ;; PCH
    pha
    lda user_context + 2         ;; PCL
    pha
    lda user_context + 1         ;; P
    pha
    ldx user_context + 4         ;; X
    ldy user_context + 5         ;; Y
    lda user_context + 5         ;; Ac
    pha                          ;; for use in the user return stub 
    txa
    pha

    ;; install in user return stub, and jump to it
    ldx #$00          
@stub_loop:
    lda in_user_return_stub, x     
    sta USER_RTI, x      
    inx
    cpx #(in_user_return_stub_end - in_user_return_stub)         
    bne @stub_loop
    jmp USER_RTI     

in_user_return_stub:
    lda user_page_table + 15
    sta MMU_PAGE_TABLE  + 15
    pla
    tax
    pla
    rti   
in_user_return_stub_end:   

.global _life_raft
_life_raft:
user_context:
    .res 8 
user_page_table:
    .res 16
kernel_page_table: 
    .res 16