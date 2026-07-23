
#include "comman.h"

uint8_t free_frames[256];
uint8_t free_top = 0;
uint8_t kernel_page_table[16];

void kalloc_init(void){
    uint16_t i;
    free_top = 0;
    for(i = 0; i < 16; i++){
        kernel_page_table[i] = (uint8_t)i;
    }
    for (i = 16; i < 256; i++){
        free_frames[free_top] = (uint8_t)i;
        free_top++;
    }
}

// O(1) allocation
uint8_t kalloc(void) {
    if (free_top == 0) {
        return 0;
    }
    free_top--;
    return free_frames[free_top];
}

// O(1) freeing
void kfree(uint8_t frame) {
    free_frames[free_top] = frame;
    free_top++;
}