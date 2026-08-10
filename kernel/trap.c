
#include "comman.h"

/*
Kernel BRK handler: The central system call dispatcher for xv6502.
Acts as the primary gateway between isolated user space and the kernel.

Triggered by the software interrupt (BRK) instruction. It expects the 
system call number to be passed in the Y register. Hardware interrupts 
are explicitly re-enabled during execution to ensure long-running 
syscalls (like disk I/O) do not stall the system.

Syscalls returns:
    A 16-bit status code or value to the user process via the A (low) 
    and X (high) registers.
*/
void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    kernel_prologue();

    // syscalls take a long time so we enable interrupts here
    INTER_ON();

    // get the syscall and call it
    sys_number = proc_get_y(current_process);
    syscall = syscalls_table[sys_number];
    if(!syscall){
        LOG();
        proc_set_ax(current_process, BADSYSCALL);
        sys_exit();
    }
    else{
        // NOTE: it is the responsibility of the system call to fetch and parse it's arguments
        return_value = syscall();
    }

    proc_set_ax(current_process, return_value);

    kernel_epilogue();
    // return to irq_handler in trampoline.s that called it, 
    // and the trampoline will jump to: return_from_trap and resume the user processs
}

/*
Kernel NMI handler: simulates system exceptions and hard faults in xv6502.
Handles memory access violations, illegal user-space privilege instructions, 
and invalid opcodes. The MMU is the sole source of NMIs in this architecture.
*/
void kernel_nmi(void){

    uint8_t cause;
    uint8_t watchdog;
    uint16_t bad_va;
    Context* ctx;
    const char* mnemonic;

    kernel_prologue();

    ctx = (Context*)user_context;

    cause    = MMIO8(MMU_CAUSE_REGISTER);
    watchdog = MMIO8(MMU_WATCHDOG_REGISTER);
    bad_va   = MMIO16(MMU_VA_REGISTER_LOW);

    // acknowledge the NMI by clearing the cause register in the hardware (for nested NMIs)
    MMIO8(MMU_CAUSE_REGISTER) = 0;

    if(cause & MMU_CAUSE_V){
        LOG("proc %d terminated due to memory excess violation <%p> in pc=%p", proc_get_pid(current_process), bad_va, ctx->pc);
    }
    else if(cause & MMU_CAUSE_X){
        panic("X");
    }
    else if(cause & MMU_CAUSE_W){
        panic("W");
    }
    else if(cause & MMU_CAUSE_R){
        panic("R");
    }

    else if(cause & MMU_CAUSE_PRIVILEGE){
        switch (watchdog)
        {
            case 0x78:
                mnemonic = "SEI";
                break;
            case 0x28:
                mnemonic = "PLP";
                break;
            case 0x40:
                mnemonic = "RTI";
                break;

            default:
                panic("privilege");
        }
        LOG("proc %d terminated due to a privilege level violation \"%s\" in PC=%p", proc_get_pid(current_process), mnemonic, ctx->pc);
    }

    else if(cause & MMU_CAUSE_INVALID_OPCODE){
        LOG("pid %d terminated due to an invalid opcode <0x%x> in PC=%p", proc_get_pid(current_process), watchdog, ctx->pc);
    }

    else{
        panic("unknown NMI");
    }

    proc_set_ax(current_process, cause);
    sys_exit();   
    // no return 
}

/*
Kernel IRQ handler: The primary hardware interrupt entry point from user space.
Implements a modern split top-half/bottom-half interrupt architecture.

The top-half executes with hardware interrupts disabled to swiftly service 
critical devices via the PLIC. The bottom-half safely re-enables interrupts 
and manages higher-level system tasks, such as evaluating the current 
process's time quantum and yielding the CPU to the preemptive scheduler 
if the time slice has been exhausted.
*/
void kernel_irq(void){
    uint8_t which_device;

    if(!current_process){
        panic("kernel_irq");
    }

    kernel_prologue();

    // top half of the interrupt handler, we run it with interrupts disable
    which_device = device_interrupt();

    // bottom half of the interrupt handler, so we alow interrupts from here
    INTER_ON();

    // Timer
    if(which_device & PLIC_PIN_TIMER){
        /* 
        the kernel timer interrupt handler, 
        if the process has any quantum remaining, it will return to user space here.
        otherwise, it will by pass to the scheduler() and yield the CPU.
        */
        if(proc_ticks_dec(current_process) == 0){
            INTER_OFF();
            proc_set_state(current_process, PROC_STATE_READY);
            scheduler();
            INTER_ON();
        }
        // process has quantum remaining, so we will return to it
    }

    kernel_epilogue();

    // return to irq_handler in trampoline.s that called it, 
    // and the trampoline will jump to: return_from_trap and resume the user processs
}

/*
Device Interrupt Dispatcher: Interrogates the PLIC to route hardware IRQs.
Acts as the fast, top-half interrupt handler that interacts directly with 
hardware drivers (e.g., ticking the system clock or buffering UART bytes).

Crucially, this function is designed to be re-entrant and safe to call 
from both user-space traps and nested kernel-space interrupts. 

Returns:
    A bitfield indicating which specific devices triggered an interrupt, 
    allowing the caller to perform deferred bottom-half processing.
*/
uint8_t device_interrupt(void){

    uint8_t which_device = MMIO8(PLIC_INTERRUPT_LINES);

    // Timer
    if(which_device & PLIC_PIN_TIMER){
        timer_interrupt();
    }

    // UART
    if(which_device & PLIC_PIN_UART_RX){
        uart_rx_interrupt();
    }

    return which_device;
}

// global counter to track how deep we are in nested critical sections
static uint8_t interrupt_depth = 0;

// always disable hardware IRQ's
void interrupts_push(void) {

    INTER_OFF();
    
    interrupt_depth++;

    if (interrupt_depth == 0) {
        panic("interrupts_push");
    }
}

// only re-enable hardware interrupts if we have fully exited all nested critical sections
void interrupts_pop(void) {
    if (interrupt_depth == 0) {
        panic("interrupts_pop");
    }

    interrupt_depth--;
    
    if (interrupt_depth == 0) {
        INTER_ON();
    }
}

void kernel_prologue(void){

    if(interrupt_depth != 0){
        panic("kernel_prologue");
    }

    if(proc_get_killed(current_process) != 0 && proc_get_pid(current_process) != 1){
        LOG();
        proc_set_a(current_process, SIGKILL);
        sys_exit();
    }

    // load the process's CPU context and page table FROM the trap frame "Life Raft"
    // NOTE: the kernel stack frame and the kernel hardware stack pointer (KSP) of the process,
    // are installed by the _nmi_handler() and _irq_handler() assembly trampoline.s routines

    proc_set_ctx(current_process, (Context*)user_context);
    
    proc_set_page_table(current_process, user_page_table);

    memcpy(proc_get_kernel_low_memory(current_process), kernel_page_table, 3);

    // NOTE: not enabling interrupts here yet!
}

void kernel_epilogue(void){

    INTER_OFF();

    if(interrupt_depth != 0){
        panic("kernel_epilogue");
    }

    if(proc_get_killed(current_process) != 0 && proc_get_pid(current_process) != 1){
        LOG();
        proc_set_a(current_process, SIGKILL);
        sys_exit();
    }
    
    // save the process's CPU context and page table INTO the trap frame "Life Raft"
    // NOTE: not installing the kernel stack frame, it is just saving it to the trampoline!
    // so it can be loaded back to the CPU and to the MMU in the _nmi_handler() and _irq_handler()

    proc_get_ctx(current_process, (Context*)user_context);
    
    proc_get_page_table(current_process, user_page_table);

    memcpy(kernel_page_table, proc_get_kernel_low_memory(current_process), 3);
}
