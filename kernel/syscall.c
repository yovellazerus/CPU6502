
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
    syscalls_table[SYS_WRITE] = sys_write;
    syscalls_table[SYS_FORK]  = sys_fork;
    syscalls_table[SYS_EXIT]  = sys_exit;
    // ...
}

// fetch the raw argument int AX (X is High, A is Low)
static uint16_t fetch_raw(Proc* p){
    uint8_t a;
    uint8_t x;
    uint16_t ax;
    a = (uint8_t)proc_get_ctx(p)->a;
    x = (uint8_t)proc_get_ctx(p)->x;
    ax = ((uint16_t)x << 8) | a;
    return ax;
}

// TODO: sys_* implementation should be in dedicated files
static uint8_t kernel_buffer[256];
int sys_write(void){
    SyscallArg syscall_arg;
    uint16_t ax;
    const uint8_t* user_page_table;

    memset(kernel_buffer, 0, sizeof(kernel_buffer));

    ax = fetch_raw(current_process);

    user_page_table = proc_get_page_table(current_process);

    // populate the system call argument
    if(copy_from_user(&syscall_arg, ax, sizeof(syscall_arg), user_page_table) < 0){
        printk("syscall_arg pointer out of user space!\n");
        return -1;
    }

    if(syscall_arg.write.size >= sizeof(kernel_buffer)){
        printk("write syscall is limited to %d bytes for now...", sizeof(kernel_buffer));
        return -1;
    }

    // take the data from the user pointer to the kernel buffer
    if(copy_from_user(kernel_buffer, (uint16_t)syscall_arg.write.buffer, syscall_arg.write.size, user_page_table) < 0){
        printk("user pointer out of user space!");
        return -1;
    }

    // TODO: console write
    printk("%s", kernel_buffer);

    // TODO: real "write" return value
    return ('X' << 8) | 'A'; 
}
