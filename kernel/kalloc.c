
#include "comman.h"

static uint8_t free_frames[256];
static uint8_t free_top = 0;

void kalloc_init(void){
    uint16_t i;
    free_top = 0;
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
        kernel_page_table[i] = MMIO8(MMU_PAGE_TABLE + i);
    }
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
    if(frame == FRAME_UNUSED) return;
    free_frames[free_top] = frame;
    free_top++;
}