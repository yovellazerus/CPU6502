
#include "comman.h"

extern uint8_t life_raft[PAGE_TABLE_SIZE];

Proc proc_table[MAX_PROC_COUNT];
Proc* current_proc;

uint16_t next_pid = 1; // global pid counter

Proc* palloc(void) {
    Proc* p = 0;
    uint8_t i;
    uint8_t seg0_frame;

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

    seg0_frame = kalloc();
    if (seg0_frame == 0) {
        p->state = PROC_STATE_UNUSED;
        return 0;
    }
    
    p->page_table[0] = seg0_frame;

    // set the proc only frame to all zeros's
    MMIO8(MMU_PAGE_TABLE + 1) = seg0_frame;
    memset((void*)WINDOW1, 0, 4096);

    return p;
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

// Copy the process's CPU context into the Trap Segment "Life Raft"
static void copy_to_life_raft(const Context* ctx){
    memcpy(life_raft, ctx, sizeof(*ctx));
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

            copy_to_life_raft(&p->ctx);

            // no return
            //return_from_trap(p->page_table); 
        }

        round_robin_index++;
        if (round_robin_index >= MAX_PROC_COUNT) {
            round_robin_index = 0;
        }
    }
}