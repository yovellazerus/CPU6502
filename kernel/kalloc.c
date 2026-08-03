
#include "comman.h"

static uint8_t free_frames[256];
static uint8_t free_top = 0;

void kalloc_init(void){
    uint16_t i;
    free_top = 0;
    // push all the physical RAM frames to the free pool (not including the kernel frames) 
    for (i = 255; i >= PAGE_TABLE_SIZE; i--){
        free_frames[free_top] = (uint8_t)i;
        free_top++;
    }
}

uint8_t kalloc(void) {
    uint8_t frame;
    uint8_t old_frame;
    void*   buffer;

    if (free_top == 0) {
        return FRAME_UNUSED;
    }
    free_top--;
    frame = free_frames[free_top];

    // clear the memory for use
    buffer = mmu_map_window(1, frame, &old_frame);
    if(!buffer){
        kfree(frame);
        return FRAME_UNUSED;
    } 
    memset(buffer, 0, 4096);
    mmu_unmap_window(1, old_frame);

    return frame;
}

void kfree(uint8_t frame) {
    uint8_t old_frame;
    void*   buffer;

    // guarding from freeing empty slots, and freeing static kernel frames
    if(frame == FRAME_UNUSED || frame < PAGE_TABLE_SIZE){
        return;
    }

    free_frames[free_top] = frame;
    free_top++;

    // fill the freed frame with junk
    buffer = mmu_map_window(1, frame, &old_frame);
    if(!buffer){
        return;
    } 
    memset(buffer, 'F', 4096);
    mmu_unmap_window(1, old_frame);
}