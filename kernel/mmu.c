
#include "common.h"

void mmu_init(void) {
    uint16_t i;

    // mapout the ROM, and install the irq/nmi kernel handlers
    MMIO8(ROM_ENABLE) = ROM_ENABLE_FALSE;
    MMIO16(0xfffa) = (uint16_t)nmi_handler;
    MMIO16(0xfffe) = (uint16_t)kernel_vector;

    // ensure the windows start in their default state
    MMIO8(MMU_PAGE_TABLE + 1) = 1;
    MMIO8(MMU_PAGE_TABLE + 2) = 2;

    // construct the kernel global page table
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
        MMIO8(MMU_PAGE_TABLE + i) = (uint8_t)i;
        kernel_page_table[i] = (uint8_t)i;
    }
}

void* mmu_map_window(uint8_t window, frame_t frame, frame_t* out_old_frame){
    if (window != 1 && window != 2) panic("mmu_map_window"); 

    // pull the old frame from the shadow table
    *out_old_frame = kernel_page_table[window];
    
    // update hardware and shadow table
    MMIO8(MMU_PAGE_TABLE + window) = frame;
    kernel_page_table[window] = frame;

    // update the active process's tracking
    if(current_process != NULL) {
        current_process->kernel_low_memory[window] = frame;
    }

    return (window == 1) ? (void*)WINDOW1 : (void*)WINDOW2;
}

void mmu_unmap_window(uint8_t window, frame_t old_frame){
    if (window != 1 && window != 2) panic("mmu_unmap_window");

    // restore hardware and shadow table
    MMIO8(MMU_PAGE_TABLE + window) = old_frame;
    kernel_page_table[window] = old_frame;

    // restore the active process's tracking
    if(current_process != NULL) {
        current_process->kernel_low_memory[window] = old_frame;
    }
}
