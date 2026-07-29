
#include "comman.h"

/* 
the kernel syscall entry.
*/
void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    // load process ctx from "Life Raft"
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
    
}

/*
Kernel NMI handler, it serves as a "watchdog" for security purposes.
it's job is to terminate a user process with flag "I" set.
the "watchdog" is the only source for NMI's in this system
*/
void kernel_nmi(void){
    kernel_prologue();
    printk("kernel: prosess \"%s\" [%d] was terminated do to having \"I\" flag set\n", proc_get_name(current_process), proc_get_pid(current_process));
    proc_set_ax(current_process, IFLAGEON);
    sys_exit();    
}

/*
This function handles hardware interrupts from system devices,
it differs from device_interrupt() in that it is called from user space, 
whereas device_interrupt() can also be called from an IRQ in kernel space.
*/
void kernel_irq(void){

    if(!current_process){
        panic("is this possible?");
    }

    // load process ctx from "Life Raft"
    kernel_prologue();

    // if true it is the timer
    if(device_interrupt()){
        /* 
        the kernel timer interrupt handler, 
        If the process has any quantum remaining, it will return to user space here.
        Otherwise, it will by pass to the scheduler() and yield the CPU.
        */
        if(proc_ticks_dec(current_process) == 0){

            proc_set_state(current_process, PROC_STATE_READY);

            scheduler();
        }
    }
    // else, not the timer

    // process has quantum remaining, so we will return to it
    kernel_epilogue();
    
}

// this is doing the work of a device IRQ
// TODO: only Timer IRQ's for now
bool device_interrupt(void){

    // increment global system timer
    if(++systicks == 0) panic("systicks");

    return true; // for timer interrupt

}

// can be used for a kernel debugare?
void kernel_software_interrupt(void){
    panic("kernel break point");
}
