
.include "..\cc65-snapshot-win64\asminc\zeropage.inc"
.include "..\machine\machine.inc"

.segment "TRAMPOLINE"

;; form ca65
.import popax

;; form kernel.cfg
.import __STACK_START__

;; from trap.c
.import _kernel_brk
.import _kernel_irq
.import _kernel_nmi
.import _device_interrupt

;; from debugger.c
.import _kernel_debugger

;; from proc.c
.import _kernel_epilogue

.global _irq_handler
_irq_handler:

    ;; decimal mode is not used in the kernel
    cld

    ;; save user CPU registers to the life raft
    sta _user_context + 6    ;; A
    sty _user_context + 5    ;; Y
    stx _user_context + 4    ;; X
    pla
    sta _user_context + 1    ;; P 
    pla
    sta _user_context + 2    ;; PCL
    pla
    sta _user_context + 3    ;; PCH 
    tsx
    stx _user_context + 0    ;; SP

    ;; switch to the process kernel hardware stack (aka KSP)
    ldx _user_context + 7
    txs

    ;; switch the memory map to kernel space
    ldx #$00          
@mmu_loop:
    lda _kernel_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ;; have we done all the segments except the last one?
    bne @mmu_loop

    ;; save the user last segment from the MMU prev register to the "life raft"
    lda MMU_PREV_REGISTER
    sta _user_page_table, x

    ;; save the kernel last segment to prev register to be used by "RTI" in KERNEL space!
    lda _kernel_page_table, x
    sta MMU_PREV_REGISTER

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5
    
    ;; jmp to C functions in the kernel
    lda _user_context + 1   ;; load P
    and #%00010000         ;; check the "B" flag
    beq @irq
    jsr _kernel_brk
    jmp _return_from_trap
@irq:
    jsr _kernel_irq
    jmp _return_from_trap
    

.global _nmi_handler
_nmi_handler:

    ;; decimal mode is not used in the kernel
    cld

    ;; save user CPU registers to the life raft
    sta _user_context + 6    ;; A   
    sty _user_context + 5    ;; Y
    stx _user_context + 4    ;; X
    pla
    sta _user_context + 1    ;; P
    pla
    sta _user_context + 2    ;; PCL
    pla
    sta _user_context + 3    ;; PCH
    tsx
    stx _user_context + 0    ;; SP

    ;; switch to the user kernel hardware stack (aka KSP)
    ldx _user_context + 7
    txs

    ;; switch the memory map to kernel space
    ldx #$00          
@mmu_loop:
    lda _kernel_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ;; have we done all the segments except the last one?
    bne @mmu_loop

    ;; save the user last segment from the MMU prev register to the "life raft"
    lda MMU_PREV_REGISTER
    sta _user_page_table, x

    ;; save the kernel last segment to prev register to be used by "RTI" in KERNEL space!
    lda _kernel_page_table, x
    sta MMU_PREV_REGISTER

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5

    ;; jmp to C function in the kernel
    jsr _kernel_nmi
    jmp _return_from_trap

;; *********************************    Core system assembly routine    ********************************************
;; 1) install the user _irq_handler to the IRQ vector.
;; 2) load the user page table from the "lift raft" to the MMU. 
;; 3) load the user last segment from the "lift raft" to the MMU prev register
;; 4) save the process kernel hardware stack (KSP) to the "life raft"
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
    lda _user_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ;; have we done all the segments except the last one?
    bne @mmu_loop
    lda _user_page_table, x   ;; the user last segment
    sta MMU_PREV_REGISTER    ;; will be installed in the last MMU segment by "RTI"

    ;; save the process kernel hardware stack to the "life raft"
    tsx
    stx _user_context + 7

    ;; restore CPU registers form life raft
    ldx _user_context + 0         ;; SP
    txs               
    lda _user_context + 3         ;; PCH
    pha
    lda _user_context + 2         ;; PCL
    pha
    lda _user_context + 1         ;; P
    pha
    ldx _user_context + 4         ;; X
    ldy _user_context + 5         ;; Y
    lda _user_context + 6         ;; A

    ;; restore the last user segment from MMU prev register
    rti  

STACK = $0100 + zpsavespace 
ZP_ON_STACK = zpsavespace 

;; IRQ handler for device interrupts and BRK's in the kernel
;; to execute C code we need to save the zero page registers on the kernel HARDWARE stack
;; so we can support nested kernel space interrupt's and a preemptive kernel 
;; NOTE: must be one the HARDWARE stack do to 6502 pointer behavior. 
.global _kernel_vector
_kernel_vector:
    pha
    txa
    pha
    tya
    pha

    ;;  push the zero page to the kernel HARDWARE stack
    ldx #0
@push:
    lda $0000, x
    pha
    inx
    cpx #<zpsavespace 
    bne @push

    ;; check the "B" flag
    ;; the "P" register is offset by the exact size of the number of zero page bytes we pushed
    tsx
    lda $0104 + zpsavespace , x
    and #%00010000  
    beq @irq
    jsr _kernel_debugger
    jmp @restore_zp

@irq:
    ;; now we can execute C code on the current process kernel stack frame
    jsr _device_interrupt

@restore_zp:
    ;; pop the zero page form the kernel HARDWARE stack 
    ldx #<zpsavespace 
    dex
@pop:
    pla
    sta $0000, x
    dex
    bpl @pop

@end:
    pla
    tay
    pla
    tax
    pla
    rti

;; kernel debugger core to save the CPU registers to a Context pointer given in AX
;;
;; void get_cpu_state(Context* ctx);
;;
.global _get_cpu_state
_get_cpu_state:

    ;; context pointer in AX
    sta ptr1+0
    stx ptr1+1

    tsx

    ;; skip 4 bytes of "_kernel_debugger" and "_get_cpu_state" return address 
    ;; plus 1 byte to point to the first pushed register (Y)
    inx
    inx
    inx
    inx
    inx 

    ;; load Y 
    lda STACK, x
    ldy #5    
    sta (ptr1), y

    ;; load X
    inx
    lda STACK, x
    ldy #4    
    sta (ptr1), y

    ;; load A
    inx
    lda STACK, x
    ldy #6    
    sta (ptr1), y

    ;; load P 
    inx
    lda STACK, x
    ldy #1    
    sta (ptr1), y

    ;; load PCL
    inx
    lda STACK, x
    ldy #2   
    sta (ptr1), y

    ;; load PCH 
    inx
    lda STACK, x
    ldy #3   
    sta (ptr1), y

    ;; X is now at the value of the original SP was before the "BRK" (mines the number of bytes pushed o the stack)
    txa
    clc
    adc #ZP_ON_STACK
    ldy #0
    sta (ptr1), y

    rts

;; initializes a kernel software stack pointer in the given frame (the frame in A must be allocated!)
;;
;; void make_kernel_stack(uint8_t frame);
;;
.global _make_kernel_stack
_make_kernel_stack:
    ;; save the OLD frame of segment 1 to tmp1, and map WINDOW1 to the given frame
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

    ;; store the value of __STACK_START__ to the offset of c_sp in the NEW stack frame
    lda #<__STACK_START__
    sta (ptr1), y          
    iny
    lda #>__STACK_START__
    sta (ptr1), y           

    ;; restore the OLD frame of WINDOW1
    ldx tmp1
    stx MMU_PAGE_TABLE + 1  
    rts

;; preformed the context switch form process "OLD", kernel stack to process "NEW" kernel stack.
;; 
;; void context_switch(Proc* OLD, Proc* NEW);
;;
.global _context_switch
_context_switch:

    ;; push the P register
    php

    ;; NEW in AX
    sta ptr1+0
    stx ptr1+1

    ;; OLD on software stack
    jsr popax
    sta ptr2+0
    stx ptr2+1
    
    ;; save the kernel low memory to the OLD process (offsets 1, 2, 3)
    ldy #1
    lda _kernel_page_table + 0
    sta (ptr2), y
    iny
    lda _kernel_page_table + 1
    sta (ptr2), y
    iny
    lda _kernel_page_table + 2
    sta (ptr2), y

    ;; switch hardware KSP (offset 0)
    ldy #0
    tsx
    txa
    sta (ptr2), y
    lda (ptr1), y
    tax              
    txs

    ;; load the kernel low memory from the NEW process safely (offsets 3, 2, 1)
    ldy #3
    lda (ptr1), y       
    sta MMU_PAGE_TABLE + 2
    sta _kernel_page_table + 2
    dey
    lda (ptr1), y       
    sta MMU_PAGE_TABLE + 1
    sta _kernel_page_table + 1
    dey
    lda (ptr1), y       
    sta MMU_PAGE_TABLE + 0       
    sta _kernel_page_table + 0  ;; swap the ZP last!

    ;; restore the P register
    plp 

    rts


;; preformed the context switch form process "OLD", kernel stack to process "NEW" kernel stack,
;; when "NEW" is a NEW process just created bye sys_fork() and it is it's first quantum to run.
;;
;; void first_context_switch(Proc* OLD, Proc* NEW);
;;
.global _first_context_switch
_first_context_switch:

    ;; push the P register
    php

    ;; NEW in AX
    sta ptr1+0
    stx ptr1+1

    ;; OLD on software stack
    jsr popax
    sta ptr2+0
    stx ptr2+1
    
    ;; save the kernel low memory to the OLD process (offsets 1, 2, 3)
    ldy #1
    lda _kernel_page_table + 0
    sta (ptr2), y
    iny
    lda _kernel_page_table + 1
    sta (ptr2), y
    iny
    lda _kernel_page_table + 2
    sta (ptr2), y

    ;; switch hardware KSP (offset 0)
    ldy #0
    tsx
    txa
    sta (ptr2), y
    lda (ptr1), y
    tax              
    txs

    ;; load the kernel low memory from the NEW process safely (offsets 3, 2, 1)
    ldy #3
    lda (ptr1), y       
    sta MMU_PAGE_TABLE + 2
    sta _kernel_page_table + 2
    dey
    lda (ptr1), y       
    sta MMU_PAGE_TABLE + 1
    sta _kernel_page_table + 1
    dey
    lda (ptr1), y       
    sta MMU_PAGE_TABLE + 0       
    sta _kernel_page_table + 0  ;; swap the ZP last! 

    ;; restore the P register
    plp 
    
    ;; jump directly to user space, 
    ;; because there is nowhere for the NEW process to return to in the kernel (no function called it). 
    jsr _kernel_epilogue
    jmp _return_from_trap

;; global "Life Raft" for saving and restoring user context and memory map
.global _user_context
_user_context:
    .res 8 
.global _user_page_table
_user_page_table:
    .res 16
.global _kernel_page_table
_kernel_page_table: 
    .res 16