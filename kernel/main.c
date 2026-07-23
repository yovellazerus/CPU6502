
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

Context ctx = {
    0xff,
    0x00,
    0x0200,
    0x00,
    0x00,
    0x00
};

// testing that writing to MMU is not possible from user space
uint8_t code[] = {
    0xa9, 0x42,          // lda #$42
    0x8d, 0x26, 0xfe,    // sta $SEG15
    0x4c, 0x05, 0x02,    // jmp $0205
};

void main(void) {

    Proc* p = NULL;
    int i = 0;

    hardware_init();

    printk("******* kernel v%d ********\n", 1);

    kalloc_init();
    proc_init();
    fs_init();

    run_init_process();

    start_timer();
    asm("cli");

    // user space test
    p = palloc();
    if(!p){
        panic("palloc");
    }
    for(i = 0; i < 16; i++){
        p->page_table[i] = kalloc();
        if(p->page_table[i] == FRAME_UNUSED){
            panic("no more frames");
        }
    }
    if(copy_to_user(code, 0x0200, sizeof(code), p->page_table) < 0){
        panic("copy_to_user");
    }
    copy_to_life_raft(&ctx, p->page_table, kernel_page_table);
    return_from_trap();

    // no return
    scheduler();
}
