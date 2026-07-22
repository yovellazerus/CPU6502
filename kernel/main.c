
#include "comman.h"

void hardware_init(void){

    // mapout the ROM, and install the irq/nmi kernel handlers
    MMIO8(MMU_ROM_ENABLE) = MMU_ROM_ENABLE_FALSE;
    MMIO16(0xfffa) = (uint16_t)nmi_handler;
    MMIO16(0xfffe) = (uint16_t)irq_handler;

    // ...
}

void proc_init(void){

}

void fs_init(void){

}
    
void run_init_process(void){

}

void start_timer(void){
    
}

void main(void) {

    hardware_init();

    printk("******* kernel v%d ********\n", 1);

    kalloc_init();
    proc_init();
    fs_init();

    run_init_process();

    start_timer();
    asm("cli");
    // no return
    scheduler();
}
