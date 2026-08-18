
#include "common.h"

Syscall syscalls_table[256];

// populate the system call argument union
bool syscall_populate_argument(Syscall_Argument* arg){
    uint16_t ax = proc_get_ax(current_process);
    if(copy_from_user(arg, ax, sizeof(*arg), current_process) < 0){
        return false;
    }
    return true;
}

void syscall_init(void){
    syscalls_table[SYS_FORK]   = sys_fork;
    syscalls_table[SYS_EXIT]   = sys_exit;
    syscalls_table[SYS_WAIT]   = sys_wait;
    syscalls_table[SYS_KILL]   = sys_kill;
    syscalls_table[SYS_SBRK]   = sys_sbrk;
    syscalls_table[SYS_SLEEP]  = sys_sleep;
    syscalls_table[SYS_GETPID] = sys_getpid;
    syscalls_table[SYS_CLOSE]  = sys_close;
    syscalls_table[SYS_OPEN_]  = sys_open;
    syscalls_table[SYS_WRITE]  = sys_write;
    syscalls_table[SYS_READ]   = sys_read;
    syscalls_table[SYS_IOCTL]  = sys_ioctl;
    // ...
}

