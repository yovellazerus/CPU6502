
#include "comman.h"

union SyscallArg{
    struct {
        uint16_t size;
        void* buffer;
    } write;
    // ...
};

Syscall syscalls_table[256];

void syscall_init(void){
    memset(syscalls_table, 0, sizeof(syscalls_table));
    syscalls_table[SYS_DUMP]  = sys_dump;
    syscalls_table[SYS_FORK]  = sys_fork;
    syscalls_table[SYS_EXIT]  = sys_exit;
    syscalls_table[SYS_WAIT]  = sys_wait;
    // ...
}

// console syscall demo for debug
static uint8_t kernel_buffer[256];
int sys_dump(void){
    SyscallArg syscall_arg;
    uint16_t ax;
    const uint8_t* user_page_table;

    memset(kernel_buffer, 0, sizeof(kernel_buffer));

    ax = proc_get_ax(current_process);

    user_page_table = proc_get_page_table(current_process);

    // populate the system call argument
    if(copy_from_user(&syscall_arg, ax, sizeof(syscall_arg), user_page_table) < 0){
        printk("syscall_arg pointer out of user space!\n");
        return -1;
    }

    // printk("%d\n", syscall_arg.write.size);

    if(syscall_arg.write.size >= sizeof(kernel_buffer)){
        printk("dump syscall is limited to %d bytes for now...", sizeof(kernel_buffer));
        return -1;
    }

    // take the data from the user pointer to the kernel buffer
    if(copy_from_user(kernel_buffer, (uint16_t)syscall_arg.write.buffer, syscall_arg.write.size, user_page_table) < 0){
        printk("user pointer out of user space!");
        return -1;
    }

    printk("%s", kernel_buffer);

    return ('X' << 8) | 'A'; 
}
