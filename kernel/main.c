
#include "comman.h"

void hardware_init(void){

    // mapout the ROM, and install the irq/nmi kernel handlers
    MMIO8(ROM_ENABLE) = ROM_ENABLE_FALSE;
    MMIO16(0xfffa) = (uint16_t)nmi_handler;
    MMIO16(0xfffe) = (uint16_t)kernel_vector;

    timer_init();

    // ...
}

void fs_init(void){

}

void main(void) {

    hardware_init();

    printk("******* kernel v%d ********\n", 1);

    kalloc_init();
    proc_init();
    syscall_init();
    fs_init();

    printk("kernel: starting init process...\n");
    run_init_process();
    run_extra_process();
    run_extra_process2();

    // no return
    scheduler();
}
