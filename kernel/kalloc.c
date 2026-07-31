
#include "comman.h"

static uint8_t free_frames[256];
static uint8_t free_top = 0;

void memory_init(void){
    uint16_t i;
    free_top = 0;
    // construct the kernel global page table
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
    
    // guarding from freeing empty slots, and freeing static kernel frames
    if(frame == FRAME_UNUSED || frame < PAGE_TABLE_SIZE){
        return;
    }

    free_frames[free_top] = frame;
    free_top++;

    // fill the freed frame with junk
    old_frame = MMIO8(MMU_PAGE_TABLE + 1);
    MMIO8(MMU_PAGE_TABLE + 1) = frame;
    memset((void*)WINDOW1, 'F', 4096);
    MMIO8(MMU_PAGE_TABLE + 1) = old_frame;
}