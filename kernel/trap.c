
#include "comman.h"

/* 
the kernel syscall entry.
*/
void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    kernel_prologue();

    // syscalls take a long time so we enable interrupts here
    __asm__("cli");

    // get the syscall and call it
    sys_number = proc_get_ctx(current_process)->y;
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
kernel NMI handler, it serves as a "watchdog" for security purposes.
it's job is to terminate a user process that try to execute an invalid opcode.
and in addition to make shore user processes cant set "I" flag on, and keep kernel space safe,
by prohibit user code form execute "SEI"/"PLP"/"RTI" instructions.
the "watchdog" is the only source for NMI's in this system
*/
void kernel_nmi(void){

    uint8_t watchdog = MMIO8(MMU_WATCHDOG_REGISTER);

    kernel_prologue();

    switch (watchdog)
    {
        case 0x78:
            LOG("SEI");
            break;
        case 0x28:
            LOG("PLP");
            break;
        case 0x40:
            LOG("RTI");
            break;
        
        default:
            LOG();
            // printk("\tprosess \"%s\" [%d] was terminated due to executing invalid opcode: <0x%x>\n", 
            //     proc_get_name(current_process), proc_get_pid(current_process), watchdog);
    }
    proc_set_ax(current_process, WATCHDOG);
    sys_exit();   
    // no return 
}

/*
this function handles hardware interrupts from system devices,
it differs from device_interrupt() in that it is called from user space, 
whereas device_interrupt() can also be called from an IRQ in kernel space.
*/
void kernel_irq(void){
    uint8_t which_device;

    if(!current_process){
        panic("is this possible?");
    }

    kernel_prologue();

    // top half of the interrupt handler, we run it with interrupts disable
    which_device = device_interrupt();

    // bottom half of the interrupt handler, so we alow nested interrupts from here
    __asm__("cli");

    // MMU
    if(which_device & PLIC_PIN_MMU){
        LOG();
        proc_set_ax(current_process, SEGFAULT);
        sys_exit();
    }

    // Timer
    if(which_device & PLIC_PIN_TIMER){
        /* 
        the kernel timer interrupt handler, 
        if the process has any quantum remaining, it will return to user space here.
        otherwise, it will by pass to the scheduler() and yield the CPU.
        */
        if(proc_ticks_dec(current_process) == 0){
            proc_set_state(current_process, PROC_STATE_READY);
            scheduler();
        }
        // process has quantum remaining, so we will return to it
    }

    kernel_epilogue();

    // return to irq_handler in trampoline.s that called it, 
    // and the trampoline will jump to: return_from_trap and resume the user processs
}

// This function is doing the work of a device interrupt request handler,
// returns the bitfield from the PLIC indicate which device caused the interrupt,
// for handleing the interrupt in the kernel_irq() function, for IRQ's for, user space.
// NOTE: can be called form user space or kernel code! 
uint8_t device_interrupt(void){

    uint8_t which_device = MMIO8(PLIC_INTERRUPT_LINES);

    // Timer
    if(which_device & PLIC_PIN_TIMER){
        timer_interrupt();
    }

    // MMU
    if(which_device & PLIC_PIN_MMU){
        mmu_interrupt();
    }

    // UART
    if(which_device & PLIC_PIN_UART_RX){
        uart_rx_interrupt();
    }

    return which_device;
}
