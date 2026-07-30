.include "..\cc65-snapshot-win64\asminc\zeropage.inc"

.segment "TRAMPOLINE"

;; form ca65
.importzp c_sp
.importzp tmp1
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

MMU_PAGE_TABLE      = $fe20 ;; 16 bytes
MMU_PREV_REGISTER   = $fe40
VECTORS             = $fffa

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
    cpx #$0F          ;; have we done segments 0 through 14?
    bne @mmu_loop

    ;; load the user last segment from the MMU register to the "life raft"
    lda MMU_PREV_REGISTER
    sta user_page_table + 15

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5

    ;; NOTE: for debugging purposes, this is disabled
    ;; now can take device interrupts in the kernel
    ;; cli
    
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
    cpx #$0F          ;; have we done segments 0 through 14?
    bne @mmu_loop

    ;; load the user last segment from the MMU register to the "life raft"
    lda MMU_PREV_REGISTER
    sta user_page_table + 15

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
;; 3) load the user last segment from the "lift raft" (user_page_table + 15) to the MMU prev register
;; 4) restore user context from "life raft"
;; 5) return to user space by preforming "RTI" (hardware will restore user last segment from MMU register)

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
    cpx #$0F          ;; have we done segments 0 through 14?
    bne @mmu_loop

    lda user_page_table + 15 ;; the user last segment
    sta MMU_PREV_REGISTER    ;; will be put in last segment by "RTI"

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

    ;; restore the last user segment from MMU prev register
    rti  

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
    and #%00010000  ;; check the B flag
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

;; initializes the kernel software stack pointer in the allocated frame in A
;;
;; void make_stack(uint8_t frame);
;;
.global _make_stack
_make_stack:
    ;; save the old frame of segment 1 to tmp1 and map WINDOW1 to frame in A
    ldx MMU_PAGE_TABLE + 1  
    stx tmp1
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
    ldx tmp1
    stx MMU_PAGE_TABLE + 1  
    rts

;; preformed the context switch form process "old" kernel stack to process "new" kernel stack 
;; 
;;
;; void context_switch(Proc* old, Proc* new);
;;
.global _context_switch
_context_switch:
    ;; "new" in AX
    sta ptr1+0
    stx ptr1+1
    
    ;; "old" on software stack
    jsr popax
    sta ptr2+0
    stx ptr2+1
    
    ;; save current hardware stack pointer to old->ksp (Offset 7 in Proc)
    ldy #7
    tsx
    txa
    sta (ptr2), y
    
    ;; read the new process kernel hardware stack form new->ksp (Offset 7) to A and move it to the CPU's SP
    lda (ptr1), y
    tax              
    txs

    ;; load the new process kernel stack frame  from new->kernel_stack_frame (Offset 24 in Proc) 
    ;; and install it in to the MMU (segment 0)
    ldy #24
    lda (ptr1), y
    sta MMU_PAGE_TABLE + 0        
    
    ; return to the NEW process stack! essentially, this is the context switch
    rts

;; preformed the context switch form process "old" kernel stack to process "new" kernel stack
;; when "new" is a new process just created bye sys_fork() and it is it's first quantum to run
;;
;; void first_context_switch(Proc* old, Proc* new);
;;
.global _first_context_switch
_first_context_switch:

    ;; "new" in AX
    sta ptr1+0
    stx ptr1+1

    ;; "old" on software stack
    jsr popax
    sta ptr2+0
    stx ptr2+1
    
    ; save sp to old->ksp (offset 7 in Proc)
    ldy #7
    tsx
    txa
    sta (ptr2), y
    
    ; read new frame (offset 24 in Proc), ignore ksp since it is empty
    ldy #24
    lda (ptr1), y
    
    ; swap memory
    sta MMU_PAGE_TABLE + 0
    
    ; set a fresh hardware stack pointer for the new process
    ldx #$ff
    txs
    
    ; jump directly to user space, because there is nowhere for it to return to in the kernel (no function called it) 
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