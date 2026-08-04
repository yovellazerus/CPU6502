
#include "comman.h"

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

void* mmu_map_window(uint8_t window, uint8_t frame, uint8_t* out_old_frame){
    if (window != 1 && window != 2) panic("mmu_map_window"); 
    interrupts_push();

    *out_old_frame = proc_get_kernel_low_memory(current_process)[window];
    
    MMIO8(MMU_PAGE_TABLE + window) = frame;
    proc_get_kernel_low_memory(current_process)[window] = frame;

    if(current_process != NULL) {
        proc_get_kernel_low_memory(current_process)[window] = frame;
    }

    interrupts_pop();
    return (window == 1) ? (void*)WINDOW1 : (void*)WINDOW2;
}

void mmu_unmap_window(uint8_t window, uint8_t old_frame){
    if (window != 1 && window != 2) panic("mmu_unmap_window");
    interrupts_push();

    MMIO8(MMU_PAGE_TABLE + window) = old_frame;
    proc_get_kernel_low_memory(current_process)[window] = old_frame;

    if(current_process != NULL) {
        proc_get_kernel_low_memory(current_process)[window] = old_frame;
    }

    interrupts_pop();
}

void mmu_interrupt(void) {
    
    // interrupt acknowledge
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_MMU;
    
    // the process termination will be handled by kernel_irq
}