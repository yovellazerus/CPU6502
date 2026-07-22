
#include "comman.h"

Proc proc_table[MAX_PROC_COUNT];
Proc* current_proc;      
uint8_t* MMIO_MMU_PAGE_TABLE;    // Pointer to MMU segment hardware registers

// Assembly routine that installs the page table, restores context, and executes RTI
extern void return_from_trap(uint8_t* page_table);

// Copy the process's CPU context into the Trap Segment "Life Raft"
extern void copy_to_life_raft(Context* ctx);

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
            return_from_trap(p->page_table); 
        }

        round_robin_index++;
        if (round_robin_index >= MAX_PROC_COUNT) {
            round_robin_index = 0;
        }
    }
}