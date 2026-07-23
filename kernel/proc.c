
#include "comman.h"

Proc proc_table[MAX_PROC_COUNT];
Proc* current_proc;

uint16_t next_pid = 1; // global pid counter

Proc* palloc(void){
    Proc* p = 0;
    uint8_t i;

    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (proc_table[i].state == PROC_STATE_UNUSED) {
            p = &proc_table[i];
            break;
        }
    }

    // If no slots are available, return null pointer
    if (p == 0) {
        return 0; 
    }

    p->state = PROC_STATE_USED;
    p->pid = next_pid++; 

    p->ctx.a = 0;
    p->ctx.x = 0;
    p->ctx.y = 0;
    p->ctx.p = 0;
    p->ctx.sp = 0xFF;
    p->ctx.pc = 0;
    
    p->parent = 0;
    p->wchan = 0;
    p->killed = 0;
    p->sz = 0;

    for (i = 0; i < 16; i++) {
        p->page_table[i] = FRAME_UNUSED;
    }

    return p;
}

int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, uint8_t* page_table){
    uint16_t offset;
    uint8_t seg;
    uint8_t physical_frame;
    uint16_t bytes_in_page;
    uint16_t chunk;
    uint8_t* dst_window;
    uint16_t i;
    uint8_t* src = (uint8_t*)kernel_src;
    
    while (n > 0) {
        
        seg = user_dest >> 12;
        offset = user_dest & 0x0FFF;
        
        if (seg >= 15) {
            return -1; // segfault
        }
        
        physical_frame = page_table[seg];
        if (physical_frame == 0) {
            return -1; // segfault
        }
        
        MMIO8(MMU_PAGE_TABLE + 1) = physical_frame;
        
        bytes_in_page = 4096 - offset;
        chunk = (n < bytes_in_page) ? n : bytes_in_page;

        dst_window = (uint8_t*)WINDOW1 + offset;
        
        for (i = 0; i < chunk; i++) {
            dst_window[i] = *src++;
        }
        
        n -= chunk;
        user_dest += chunk;
    }
    
    return 0; // success
}

int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, uint8_t* page_table){
    uint16_t offset;
    uint8_t seg;
    uint8_t physical_frame;
    uint16_t bytes_in_page;
    uint16_t chunk;
    uint8_t* src_window;
    uint16_t i;
    uint8_t* dst = (uint8_t*)kernel_dest;
    
    while (n > 0) {
        
        seg = user_src >> 12;
        offset = user_src & 0x0FFF;
        
        if (seg >= 15) {
            return -1; // segfault
        }
        
        physical_frame = page_table[seg];
        if (physical_frame == 0) {
            return -1; // segfault
        }
        
        MMIO8(MMU_PAGE_TABLE + 1) = physical_frame;
        
        bytes_in_page = 4096 - offset;
        chunk = (n < bytes_in_page) ? n : bytes_in_page;

        src_window = (uint8_t*)WINDOW1 + offset;
        
        for (i = 0; i < chunk; i++) {
            *dst++ = src_window[i];
        }
        
        n -= chunk;
        user_src += chunk;
    }
    
    return 0; // success
}

// Copy the process's CPU context and page table into the Trap Segment "Life Raft"
void copy_to_life_raft(const Context* ctx, uint8_t* user_page_table, uint8_t* kernel_page_table){
    memcpy(life_raft, ctx, sizeof(*ctx));
    memcpy(life_raft + 8, user_page_table, 16);
    memcpy(life_raft + 8 + 16, kernel_page_table, 16);
}

void scheduler(void) {
    
    static uint8_t round_robin_index = 0;
    Proc* p;

    while (1) {
        
        asm("cli");

        p = &proc_table[round_robin_index];
        
        if (p->state == PROC_STATE_READY) {
            
            asm("sei");
            
            p->state = PROC_STATE_RUNING;
            current_proc = p;
            
            p->ticks = QUANTUM; 

            copy_to_life_raft(&p->ctx, p->page_table, kernel_page_table);

            // no return
            return_from_trap(); 
        }

        round_robin_index++;
        if (round_robin_index >= MAX_PROC_COUNT) {
            round_robin_index = 0;
        }
    }
}