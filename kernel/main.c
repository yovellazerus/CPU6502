
#include "common.h"

void main(void) {

    // hardware initialization
    mmu_init();
    timer_init();
    uart_init();
    disk_init();

    printk("kernel: version %d.%d.%d is booting...\n", 1, 0, 0);

    kalloc_init();
    proc_init();
    buffer_init();
    syscall_init();
    file_init();
    console_init();

    printk("kernel: starting init process...\n");
    run_init_process();

    scheduler();

    panic("main");
}
