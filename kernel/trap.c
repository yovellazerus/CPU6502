
#include "comman.h"

void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    interrupts_disable();

    // appdate the current process Proc struct
    copy_from_life_raft(current_process);

    // get the syscall and call it
    sys_number = proc_get_ctx(current_process)->y;
    syscall = syscalls_table[sys_number];
    if(!syscall){
        printk("bad syscall number: 0x%x\n", sys_number);
        // TODO: terminate process
    }
    else{
        // NOTE: it is the responsibility of the system call to fetch and parse it's arguments
        return_value = syscall();
    }

    proc_set_ax(current_process, return_value);

    copy_to_life_raft(current_process);

    interrupts_enable();

    return_from_trap();
}

void kernel_irq(void){
    panic("kernel_irq");
}

void kernel_nmi(void){
    panic("kernel_nmi");
}