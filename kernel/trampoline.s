
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
    sta user_context + 6    ;; A
    sty user_context + 5    ;; Y
    stx user_context + 4    ;; X
    pla
    sta user_context + 1    ;; P 
    pla
    sta user_context + 2    ;; PCL
    pla
    sta user_context + 3    ;; PCH 
    tsx
    stx user_context + 0    ;; SP

    ;; switch to the process kernel hardware stack (aka KSP)
    ldx user_context + 7
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
    sta user_page_table, x

    ;; save the kernel last segment to prev register to be used by "RTI" in KERNEL space!
    lda _kernel_page_table, x
    sta MMU_PREV_REGISTER

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5
    
    ;; jmp to C functions in the kernel
    lda user_context + 1   ;; load P
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
    sta user_context + 6    ;; A   
    sty user_context + 5    ;; Y
    stx user_context + 4    ;; X
    pla
    sta user_context + 1    ;; P
    pla
    sta user_context + 2    ;; PCL
    pla
    sta user_context + 3    ;; PCH
    tsx
    stx user_context + 0    ;; SP

    ;; switch to the user kernel hardware stack (aka KSP)
    ldx user_context + 7
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
    sta user_page_table, x

    ;; save the kernel last segment to prev register to be used by "RTI" in KERNEL space!
    lda _kernel_page_table, x
    sta MMU_PREV_REGISTER

    ;; install kernel IRQ vector
    lda #<_kernel_vector
    sta VECTORS+4
    lda #>_kernel_vector
    sta VECTORS+5

    ;; NOTE: not enabling IRQ's in here!

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
    lda user_page_table, x     
    sta MMU_PAGE_TABLE, x      
    inx
    cpx #$0F          ;; have we done all the segments except the last one?
    bne @mmu_loop
    lda user_page_table, x   ;; the user last segment
    sta MMU_PREV_REGISTER    ;; will be installed in the last MMU segment by "RTI"

    ;; save the process kernel hardware stack to the "life raft"
    tsx
    stx user_context + 7

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

;; NO_PREEMPTIVE_KERNEL := 1
.ifdef NO_PREEMPTIVE_KERNEL

;; IRQ handler for device interrupts and BRK's in the kernel
;; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;; NOTE: for interrupt's execution in KERNEL space and it will fail if for a PREEMPTIVE kernel!
;; we must not corrupt the current process kernel stack frame.
;; so we SWAP it for the kernel stack 0 (frame 0) that was used during the booting process
;; and make it the dedicated booting/interrupts kernel stack.

STACK = $0100
ZP_ON_STACK = 0

;; must not be in $0000-$0fff because they are used for the swap!
tmp_ksp:         .res 1
tmp_stack_frame: .res 1

.global _kernel_vector
_kernel_vector:
    pha
    txa
    pha
    tya
    pha

    ;; checking the "B" flag, must be before swapping memory because the "P"
    ;; register is currently on the OLD stack
    tsx
    lda $0104, x
    and #%00010000
    bne @software_brk

@hardware_irq:

    ;; check for nested kernel interrupt's,
    ;; if the stack is already stack 0, there is no need to perform the swap.
    ;; just call the C handler
    lda MMU_PAGE_TABLE + 0
    bne @not_nested
    ;; it is nested
    jsr _device_interrupt
    jmp @end

@not_nested:

    ;; save the current hardware stack pointer
    tsx
    stx tmp_ksp

    ;; save the current kernel stack frame
    lda MMU_PAGE_TABLE + 0
    sta tmp_stack_frame

    ;; swap the stack frame to frame 0 (The dedicated kernel boot/interrupt frame)
    ;; frame 0 contains its own zero page register and hardware stack
    lda #$00
    sta MMU_PAGE_TABLE + 0

    ;; reset the hardware stack pointer
    ldx #$FF
    txs

    ;; can now safely execute C code on frame 0 without corrupting the current process
    jsr _device_interrupt

    ;; restore the old kernel stack frame
    lda tmp_stack_frame
    sta MMU_PAGE_TABLE + 0

    ;; restore the old hardware stack pointer
    ldx tmp_ksp
    txs
    jmp @end

    ;; not swapping memory for the kernel debugger 
@software_brk:
    jsr _kernel_debugger

@end:
    pla
    tay
    pla
    tax
    pla
    rti

.else

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

.endif

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
    ;; save the old frame of segment 1 to tmp1, and map WINDOW1 to the given frame
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

    ;; restore the old frame of WINDOW1
    ldx tmp1
    stx MMU_PAGE_TABLE + 1  
    rts

;; preformed the context switch form process "old" kernel stack, to process "new" kernel stack.
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

    ;; load the new process kernel stack frame from new->kernel_stack_frame (Offset 24 in Proc) 
    ;; and install it in to segment 0 of the MMU
    ldy #24
    lda (ptr1), y
    sta MMU_PAGE_TABLE + 0        
    
    ; return on the NEW process stack! essentially, this is the context switch
    rts

;; preformed the context switch form process "old", kernel stack to process "new" kernel stack,
;; when "new" is a new process just created bye sys_fork() and it is it's first quantum to run.
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
    
    ;; save sp to old->ksp (offset 7 in Proc)
    ldy #7
    tsx
    txa
    sta (ptr2), y
    ;; set a fresh hardware stack pointer for the new process
    ldx #$ff
    txs
    
    ;; load the new process kernel stack frame from new->kernel_stack_frame (Offset 24 in Proc) 
    ;; and install it in to segment 0 of the MMU 
    ldy #24
    lda (ptr1), y
    sta MMU_PAGE_TABLE + 0
    
    ;; jump directly to user space, 
    ;; because there is nowhere for the new process to return to in the kernel (no function called it). 
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