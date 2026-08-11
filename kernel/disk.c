
#include "comman.h"

void disk_init(void){
    // ...
}

void disk_block_read(Block_Buffer* b){
    (void)b;
    panic("disk_block_read");
}

void disk_block_write(Block_Buffer* b){
    (void)b;
    panic("disk_block_write");
}


void disk_interrupt(void){

    // acknowledge the interrupt
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_DISK;

}