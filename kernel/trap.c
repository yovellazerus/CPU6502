
#include "comman.h"

// a toy example for a syscall from user space! 
void kernel_brk(void){
    uint8_t old_frame;
    uint8_t a;
    uint8_t x;
    uint16_t logical_ptr;
    uint8_t user_seg;
    uint16_t offset;
    uint8_t physical_frame;
    char* str;
    
    // X is High, A is Low
    a = life_raft[6];
    x = life_raft[4];
    logical_ptr = ((uint16_t)x << 8) | a;

    user_seg = logical_ptr >> 12;
    offset = logical_ptr & 0x0FFF;

    physical_frame = proc_get_frame(user_seg);

    old_frame = MMIO8(MMU_PAGE_TABLE + 1);
    MMIO8(MMU_PAGE_TABLE + 1) = physical_frame;

    str = (char*)(WINDOW1 + offset);

    while (*str != '\0') {
        putc(*str);
        str++;
    }

    MMIO8(MMU_PAGE_TABLE + 1) = old_frame;

    return_from_trap();
}

void kernel_irq(void){
    panic("kernel_irq");
}

void kernel_nmi(void){
    panic("kernel_nmi");
}