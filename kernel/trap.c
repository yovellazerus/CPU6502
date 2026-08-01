
#include "comman.h"

/* 
the kernel syscall entry.
*/
void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    kernel_prologue();

    // get the syscall and call it
    sys_number = proc_get_ctx(current_process)->y;
    syscall = syscalls_table[sys_number];
    if(!syscall){
        printk("kernel: \"%s\" [%d] terminated do to invalid syscall number: 0x%x\n", proc_get_name(current_process), proc_get_pid(current_process), sys_number);
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

    const char* mnemonic;
    uint8_t watchdog = MMIO8(MMU_WATCHDOG_REGISTER);

    kernel_prologue();

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
            printk("kernel: prosess [%d] \"%s\" was terminated do to executing invalid opcode: <0x%x>\n", 
                proc_get_pid(current_process), proc_get_name(current_process), watchdog);
            goto end;
    }
    printk("kernel: prosess [%d] \"%s\" was terminated do to executing \"%s\" instruction\n", 
            proc_get_pid(current_process), proc_get_name(current_process), mnemonic);
end:
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

    if(!current_process){
        panic("is this possible?");
    }

    kernel_prologue();

    // if true it is the timer
    if(device_interrupt()){
        /* 
        the kernel timer interrupt handler, 
        if the process has any quantum remaining, it will return to user space here.
        otherwise, it will by pass to the scheduler() and yield the CPU.
        */
        if(proc_ticks_dec(current_process) == 0){

            proc_set_state(current_process, PROC_STATE_READY);

            scheduler();
        }
    }
    // else, not the timer

    // process has quantum remaining, so we will return to it
    kernel_epilogue();

    // return to irq_handler in trampoline.s that called it, 
    // and the trampoline will jump to: return_from_trap and resume the user processs
}

// this is doing the work of a device IRQ
// TODO: only Timer IRQ's for now
bool device_interrupt(void){

    // increment global system timer
    if(++systicks == 0) panic("systicks");

    return true; // for timer interrupt
}
