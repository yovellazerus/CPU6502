
#include "comman.h"

typedef union Syscall_Arg{
    struct {
        uint16_t size;
        void* buffer;
    } write;
    // ...
} Syscall_Arg;

typedef int (*Syscall)(void);

#define SYS_WRITE 0x42
int sys_write(void);

Syscall syscalls_table[256];
uint8_t kernel_buffer[256];

void syscall_init(void){
    memset(syscalls_table, 0, sizeof(syscalls_table));
    syscalls_table[SYS_WRITE] = sys_write;
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

int sys_write(void){
    Syscall_Arg syscall_arg;
    uint16_t ax;
    const uint8_t* user_page_table;

    ax = fetch_raw(current_process);

    user_page_table = proc_get_page_table(current_process);

    // populate the system call argument
    if(copy_from_user(&syscall_arg, ax, sizeof(syscall_arg), user_page_table) < 0){
        printk("syscall_arg pointer out of user space!");
        return -1;
    }

    if(syscall_arg.write.size >= sizeof(kernel_buffer)){
        printk("write syscall is limited to 256 bytes for now...");
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

void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    interrupts_disable();

    // appdate the current process Proc struct
    copy_from_life_raft(current_process);

    // get the syscall number and call it
    sys_number = proc_get_ctx(current_process)->y;
    syscall = syscalls_table[sys_number];
    if(!syscall){
        printk("bad syscall number: 0x%x\n", sys_number);
        // TODO: terminate process
    }
    else{
        // NOTE: it is the responsibility of the system call to populate the syscall argument
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