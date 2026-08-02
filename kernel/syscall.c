
#include "comman.h"

Syscall syscalls_table[256];

void syscall_init(void){
    memset(syscalls_table, 0, sizeof(syscalls_table));
    syscalls_table[SYS_FORK]   = sys_fork;
    syscalls_table[SYS_EXIT]   = sys_exit;
    syscalls_table[SYS_WAIT]   = sys_wait;
    syscalls_table[SYS_KILL]   = sys_kill;
    syscalls_table[SYS_SBRK]   = sys_sbrk;
    syscalls_table[SYS_CLOSE]  = sys_close;
    syscalls_table[SYS_OPEN]   = sys_open;
    syscalls_table[SYS_WRITE]  = sys_write;
    syscalls_table[SYS_READ]   = sys_read;
    // ...
}

