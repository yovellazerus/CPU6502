
#include "comman.h"

static uint8_t free_frames[256];
static uint8_t free_top = 0;

void memory_init(void){
    uint16_t i;
    free_top = 0;
    // construct the kernel global page table
    // with seg0 is frame 0 aka stack0 to be used before the first process was created
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
        kernel_page_table[i] = MMIO8(MMU_PAGE_TABLE + i);
    }
    // push all the rest of the physical RAM frames to the free pool 
    for (i = 255; i >= PAGE_TABLE_SIZE; i--){
        free_frames[free_top] = (uint8_t)i;
        free_top++;
    }
}

uint8_t kalloc(void) {
    if (free_top == 0) {
        return FRAME_UNUSED;
    }
    free_top--;
    return free_frames[free_top];
}

void kfree(uint8_t frame) {
    uint8_t old_frame;
    if(frame == FRAME_UNUSED) return;
    free_frames[free_top] = frame;
    free_top++;

    // fill the freed frame with junk
    old_frame = MMIO8(MMU_PAGE_TABLE + 1);
    MMIO8(MMU_PAGE_TABLE + 1) = frame;
    memset((void*)WINDOW1, 0x42, 4096);
    MMIO8(MMU_PAGE_TABLE + 1) = old_frame;
}