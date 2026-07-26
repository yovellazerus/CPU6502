
#include "comman.h"

/* 
the kernel syscall entry, 
fast call return here (then return to user space), 
and sleeping syscalls will yield the CPU by doing:
    sleep() -> scheduler() -> (different process) -> return to user space
*/
void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    // appdate the current process Proc struct
    kernel_prologue();

    // get the syscall and call it
    sys_number = proc_get_ctx(current_process)->y;
    syscall = syscalls_table[sys_number];
    if(!syscall){
        printk("bad syscall number: 0x%x\n", sys_number);
        proc_set_ax(current_process, BADSYSCALL);
        sys_exit();
    }
    else{
        // NOTE: it is the responsibility of the system call to fetch and parse it's arguments
        // for a blocking syscall, like: sys_read, sys_write, sys_wait...
        // the process will call sleep() that will transfer control to the scheduler() and wont retrun.
        return_value = syscall();
    }

    // fast syscall return back here to resume the process
    proc_set_ax(current_process, return_value);

    kernel_epilogue();
    
    return_from_trap();
}

/* 
the kernel timer interrupt handler, 
If the process has any quantum remaining, it will return to user space here.
Otherwise, it will by pass to the scheduler() and yield the CPU.
In addition, it serves as a "watchdog" for security purposes.
Only source of NMI in this system is the Timer. 
*/
void kernel_nmi(void){

    // increment global system timer
    if(++systicks == 0) panic("systicks");

    if(!current_process){
        panic("is this possible?");
    }

    // load process ctx from "Life Raft"
    kernel_prologue();

    // the "watchdog" trait
    // only check the 'I' flag if the process was actively running in user memory (e.g., $0200 to $efff)
    // TODO: try to remove it...
    if((proc_get_ctx(current_process)->p & P_I) && 
        proc_get_ctx(current_process)->pc >= 0x0200 && 
        proc_get_ctx(current_process)->pc < 0xF000) {
        
        printk("user process cant have 'I' flage on\n");
        proc_set_ax(current_process, IFLAGEON);
        sys_exit();
    }

    if(proc_ticks_dec(current_process) == 0){

        proc_set_state(current_process, PROC_STATE_READY);

        scheduler();
    }

    // process has quantum remaining, so we will return to it
    kernel_epilogue();
    
    return_from_trap();
}

/*
This function handles hardware interrupts from system devices,
it differs from device_interrupt() in that it is called from user space, 
whereas device_interrupt() can also be called from an IRQ in kernel space.
*/
void kernel_irq(void){

    // load process ctx from "Life Raft"
    kernel_prologue();

    device_interrupt();

    kernel_epilogue();
    
    return_from_trap();
}

// this is doing the work of a device IRQ
void device_interrupt(void){
    panic("device_interrupt");
}

// can be used for a kernel debugare?
void kernel_software_interrupt(void){
    panic("kernel break point");
}
