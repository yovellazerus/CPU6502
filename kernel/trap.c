
#include "comman.h"

typedef union Syscall_Arg{
    struct {
        uint16_t size;
        void* buffer;
    } write;
    // ...
} Syscall_Arg;

uint8_t kernel_buffer[256];

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

// a toy example for a syscall from user space! 
void kernel_brk(void){
    uint16_t ax;
    const uint8_t* user_page_table;
    Syscall_Arg syscall_arg;

    interrupts_disable();
    
    // appdate the current process Proc struc
    copy_from_life_raft(current_process);

    ax = fetch_raw(current_process);

    user_page_table = proc_get_page_table(current_process);

    // populate the system call argument
    if(copy_from_user(&syscall_arg, ax, sizeof(syscall_arg), user_page_table) < 0){
        printk("syscall_arg pointer out of user space!");
        goto abort;
    }

    if(syscall_arg.write.size >= sizeof(kernel_buffer)){
        printk("write syscall is limited to 256 bytes for now...");
        goto abort;
    }

    // take the data from the user pointer to the kernel buffer
    if(copy_from_user(kernel_buffer, (uint16_t)syscall_arg.write.buffer, syscall_arg.write.size, user_page_table) < 0){
        printk("user pointer out of user space!");
        goto abort;
    }

    // TODO: console write
    printk("%s", kernel_buffer);

abort:
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