
.segment "ZEROPAGE"

tmp_old: .res 2
tmp_new: .res 2

.segment "TRAMPOLINE"

;; form ca65
.importzp c_sp
.importzp ptr1
.importzp ptr2

.import popax

;; form linker
.import __STACK_START__

.import _kernel_brk
.import _kernel_irq
.import _kernel_nmi

.import _kernel_software_interrupt
.import _device_interrupt
.import _kernel_epilogue

MMU_PAGE_TABLE = $fe20 ;; 16 bytes
USER_RTI       = $0100 ;; running in user space to close seg15 and resume user code
VECTORS        = $fffa

.global _irq_handler
_irq_handler:

    ;; save user CPU registers to the life raft
    sta user_context + 6    ;; A
    sty user_context + 5    ;; Y
    stx user_context + 4    ;; X
    pla
    sta user_context + 1    ;; P (Status) is always pulled first
    pla
    sta user_context + 2    ;; PCL (PC Low) is pulled second
    pla
    sta user_context + 3    ;; PCH (PC High) is pulled last
    tsx
    stx user_context + 0    ;; SP

    ;; switch to the user kernel hardware stack
    ldx user_context + 7
    txs

    ;; switch the memory map to kernel space
    ldx #$00          
@mmu_loop:
    lda _kernel_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ; have we done segments 0 through 14?
    bne @mmu_loop

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5

    ;; NOTE: for debugging purposes, disable this
    ;; now can take device interrupts in the kernel
    ; cli
    
    ;; jmp to C functions in the kernel
    lda user_context + 1   ;; load Status (P)
    and #%00010000         ;; check the B flag
    beq @irq
    jsr _kernel_brk
    jmp _return_from_trap
@irq:
    jsr _kernel_irq
    jmp _return_from_trap
    

.global _nmi_handler
_nmi_handler:

    ;; save user CPU registers to the life raft
    sta user_context + 6    ;; A   
    sty user_context + 5    ;; Y
    stx user_context + 4    ;; X
    pla
    sta user_context + 1    ;; P (Status) is always pulled first
    pla
    sta user_context + 2    ;; PCL (PC Low) is pulled second
    pla
    sta user_context + 3    ;; PCH (PC High) is pulled last
    tsx
    stx user_context + 0    ;; SP

    ;; switch to the user kernel hardware stack
    ldx user_context + 7
    txs

    ;; switch the memory map to kernel space
    ldx #$00          
@mmu_loop:
    lda _kernel_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ; have we done segments 0 through 14?
    bne @mmu_loop

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5

    ;; NOTE: not enable IRQ in here!

    ;; jmp to C function in the kernel
    jsr _kernel_nmi
    jmp _return_from_trap

;; *********************************    Core system assembly routine    ********************************************
;; 1) install the user _irq_handler to the IRQ vector.
;; 2) load the user page table to the MMU. 
;; 3) injecting the in_user_return_stub to the process hardware stack (to close the last segment safely).
;; 4) restore user context from "life raft".
;; 5) jump to in_user_return_stub that close the trap frame (the last segment) and preform the "RTI" to resume usr process. 

;;
;; void return_from_trap(void);
;;
.global _return_from_trap
_return_from_trap:  

    ;; restore the user IRQ vector
    lda #<_irq_handler
    sta VECTORS+4
    lda #>_irq_handler
    sta VECTORS+5

    ;; restore user memory space form life raft
    ldx #$00          
@mmu_loop:
    lda user_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ; have we done segments 0 through 14?
    bne @mmu_loop

    ;; inject in user return stub
    ldx #$00          
@stub_loop:
    lda in_user_return_stub, x     
    sta USER_RTI, x      
    inx
    cpx #(in_user_return_stub_end - in_user_return_stub)         
    bne @stub_loop

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
    lda user_context + 6         ;; A
    pha                          ;; for use in the user return stub 
    
    jmp USER_RTI     

in_user_return_stub:
    lda user_page_table + 15
    sta MMU_PAGE_TABLE  + 15
    pla
    rti   
in_user_return_stub_end:   

;; IRQ handler for device interrupts and BRK's in the kernel
.global _kernel_vector
_kernel_vector:
    pha
    txa
    pha
    tya
    pha

    tsx
    lda $0104, x
    and #%00010000  ;; Check the B flag
    beq @irq
    jsr _kernel_software_interrupt
    jmp @end
@irq:
    jsr _device_interrupt

@end:
    pla
    tay
    pla
    tax
    pla
    rti

;;
;; void make_stack(uint8_t frame);
;;
.global _make_stack
_make_stack:
    ;; save old_frame and map WINDOW1 to frame in A
    ldx MMU_PAGE_TABLE + 1  
    stx old_frame
    sta MMU_PAGE_TABLE + 1  

    ;; ptr1 set to the base of WINDOW1
    lda #$00
    sta ptr1+0
    lda #$10
    sta ptr1+1

    ;; use of the zp address of c_sp as the Y index 
    ldy #<c_sp 

    ;; store the value of __STACK_START__ to the offset of c_sp in the new stack frame
    lda #<__STACK_START__
    sta (ptr1), y          
    iny
    lda #>__STACK_START__
    sta (ptr1), y           

    ;; restore the old_frame of WINDOW1
    ldx old_frame
    stx MMU_PAGE_TABLE + 1  
    rts

old_frame:  .res 1

;;
;; void context_switch(Proc* old, Proc* new);
;;
.global _context_switch
_context_switch:
    ; cc65 passes the right argument 'new' in A and X
    sta tmp_new+0
    stx tmp_new+1
    
    ; The left argument 'old' is on the C param stack. Pop it.
    jsr popax
    sta tmp_old+0
    stx tmp_old+1
    
    ; 1. Save current hardware stack pointer to old->ksp (Offset 7 in Proc)
    ldy #7
    tsx
    txa
    sta (tmp_old), y
    
    ; 2. Read new->ksp (Offset 7) and new->kernel_stack_frame (Offset 24 in Proc)
    lda (tmp_new), y
    tax              ; X = new process hardware stack pointer
    ldy #24
    lda (tmp_new), y ; A = new process Segment 0 physical frame
    
    ; 3. THE BRAIN TRANSPLANT: Swap Segment 0 memory instantly!
    sta MMU_PAGE_TABLE + 0        
    
    ; 4. Restore the new process's hardware stack pointer
    txs
    
    ; 5. Return (This pulls the return address from the NEW process's stack!)
    rts

;;
;; void first_context_switch(Proc* old, Proc* new);
;;
.global _first_context_switch
_first_context_switch:
    sta tmp_new
    stx tmp_new+1
    jsr popax
    sta tmp_old
    stx tmp_old+1
    
    ; Save old->ksp
    ldy #7
    tsx
    txa
    sta (tmp_old), y
    
    ; Read new frame (Offset 24), ignore ksp since it is empty
    ldy #24
    lda (tmp_new), y
    
    ; Swap memory
    sta MMU_PAGE_TABLE + 0
    
    ; Set a fresh hardware stack for the new kernel thread
    ldx #$ff
    txs
    
    ; Jump directly to user space
    jsr _kernel_epilogue
    jmp _return_from_trap

.global _life_raft
_life_raft:
user_context:
    .res 8 
user_page_table:
    .res 16
.global _kernel_page_table
_kernel_page_table: 
    .res 16