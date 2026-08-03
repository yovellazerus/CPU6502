
#include "comman.h"

void hardware_init(void){

    // mapout the ROM, and install the irq/nmi kernel handlers
    MMIO8(ROM_ENABLE) = ROM_ENABLE_FALSE;
    MMIO16(0xfffa) = (uint16_t)nmi_handler;
    MMIO16(0xfffe) = (uint16_t)kernel_vector;

    timer_init();
    uart_init();
    // ...
}

void main(void) {

    hardware_init();

    printk("kernel: version %d.%d.%d is booting...\n", 1, 0, 0);

    memory_init();
    proc_init();
    syscall_init();
    vfs_init();

    printk("kernel: starting init process...\n");
    run_init_process();

    // no return
    scheduler();
}
