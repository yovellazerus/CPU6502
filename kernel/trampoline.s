
.segment "ZEROPAGE"

tmp_old: .res 2
tmp_new: .res 2

.segment "TRAMPOLINE"

;; form ca65
.importzp c_sp
.importzp ptr1
.importzp ptr2

.import popax

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
    stx user_context + 0   ;; SP

    ;; switch to the user kernel hardware stack
    ldx user_context + 7
    txs

    ;; switch the memory map to kernel space, 
    ;; all of them except for seg15, 
    ;; because we are currently running code on it
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
    lda user_context + 1   ;; Load Status (P)
    and #%00010000         ;; Check the B flag
    beq @irq
    jsr _kernel_brk
    jmp _return_from_trap
@irq:
    jsr _kernel_irq
    jmp _return_from_trap
    

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
    stx user_context + 0   ;; SP

    ;; switch to the user kernel hardware stack
    ldx user_context + 7
    txs

    ;; switch the memory map to kernel space, 
    ;; all of them except for seg15, 
    ;; because we are currently running code on it
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

; Assembly routine that installs the page table, restores context, install return to user stub
;
; void return_from_trap(void);
;
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
    cpx #$0F          ; Have we done segments 0 through 14?
    bne @mmu_loop

    ;; install in user return stub, and jump to it
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
;; void switch_threads(Proc* old, Proc* new);
;;
.global _switch_threads
_switch_threads:
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
;; void switch_to_new_thread(Proc* old, Proc* new);
;;
.global _switch_to_new_thread
_switch_to_new_thread:
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