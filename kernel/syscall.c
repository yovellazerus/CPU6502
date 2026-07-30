
#include "comman.h"

union SyscallArg{
    struct {
        uint16_t size;
        void* buffer;
    } print;
    // ...
};

Syscall syscalls_table[256];

void syscall_init(void){
    memset(syscalls_table, 0, sizeof(syscalls_table));
    syscalls_table[SYS_PRINT]  = sys_print;
    syscalls_table[SYS_FORK]   = sys_fork;
    syscalls_table[SYS_EXIT]   = sys_exit;
    syscalls_table[SYS_WAIT]   = sys_wait;
    // ...
}

// console syscall demo for debug
static char print_buffer[256];
int sys_print(void){
    SyscallArg syscall_arg;
    uint16_t ax;
    const uint8_t* user_page_table;

    memset(print_buffer, 0, sizeof(print_buffer));

    ax = proc_get_ax(current_process);

    user_page_table = proc_get_page_table(current_process);

    // populate the system call argument
    if(copy_from_user(&syscall_arg, ax, sizeof(syscall_arg), user_page_table) < 0){
        printk("syscall argument pointer out of user space!\n");
        return -1;
    }

    if(syscall_arg.print.size >= sizeof(print_buffer)){
        printk("print syscall is limited to %d bytes for now...", sizeof(print_buffer));
        return -1;
    }

    // take the data from the user pointer to the kernel buffer
    if(copy_from_user(print_buffer, (uint16_t)syscall_arg.print.buffer, syscall_arg.print.size, user_page_table) < 0){
        printk("user pointer out of user space!");
        return -1;
    }

    print_str(print_buffer);

    return ('X' << 8) | 'A'; 
}
