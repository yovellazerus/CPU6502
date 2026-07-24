
#include "comman.h"

enum Proc_State{
    PROC_STATE_UNUSED = 0,
    PROC_STATE_USED,
    PROC_STATE_READY,
    PROC_STATE_RUNING,
    PROC_STATE_SLEEPING,
    PROC_STATE_ZOMBIE
};

// order is importent for trampoline!
struct Context {
    uint8_t sp;
    uint8_t p;
    uint16_t pc; 
    uint8_t x;
    uint8_t y;
    uint8_t a;
};

struct Proc {

    // CPU and memory context
    Context ctx; 
    uint8_t page_table[PAGE_TABLE_SIZE];
    uint16_t top;
     
    // scheduler
    Proc_State state; 
    uint16_t pid;                   
    uint8_t  uid;           
    uint8_t  gid;           
    uint8_t  ecode;         
    uint8_t  priority;      
    uint8_t  ticks;   

    // inter process communication
    struct Proc* parent;    
    void* wchan;       
    uint8_t  killed;      

    // file system
    uint16_t cwd_inode;     
    uint8_t  fd_table[MAX_FILES_PER_PROC];          

    // debug 
    char name[16];

};

Proc proc_table[MAX_PROC_COUNT];
Proc* init_process;
Proc* current_process;

static uint16_t next_pid = 1; // global pid counter

static int pid_alloc(void){
    int pid = next_pid;
    if(pid == 0) panic("pid_alloc");
    next_pid++;
    return pid;
}

void proc_init(void){
    uint16_t i;
    memset(proc_table, 0, sizeof(proc_table));
    for(i = 0; i < ARRAY_SIZE(proc_table); i++){
        proc_table[i].state = PROC_STATE_UNUSED;
    }
    init_process = NULL;
    current_process = NULL;
    next_pid = 1;
}

// create a new Proc struct with empty page table, new pid, SP in a READY state 
Proc* palloc(void){
    Proc* p = 0;
    uint8_t i;

    for (i = 0; i < MAX_PROC_COUNT; i++) {
        if (proc_table[i].state == PROC_STATE_UNUSED) {
            p = &proc_table[i];
            break;
        }
    }
    // not found
    if (p == NULL) {
        return NULL; 
    }

    memset(p, 0, sizeof(*p));
    p->state = PROC_STATE_USED;
    p->pid = pid_alloc(); 
    p->ctx.sp = 0xff;
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
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
void copy_to_life_raft(const Context* ctx, uint8_t* user_page_table){
    memcpy(life_raft, ctx, sizeof(*ctx));
    memcpy(life_raft + 8, user_page_table, 16);
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
            current_process = p;
            
            p->ticks = QUANTUM; 

            copy_to_life_raft(&p->ctx, p->page_table);

            // no return
            return_from_trap(); 
        }

        round_robin_index++;
        if (round_robin_index >= MAX_PROC_COUNT) {
            round_robin_index = 0;
        }
    }
}

// TODO: not real implementation, just a test0
void run_init_process(void){

    Context ctx = {
       0xff,   // SP
       'P',    // P
       0x0200, // PC
       'X',    // X
       'Y',    // Y
       'A'     // A
    };

    // for testing, not the real init code
    uint8_t init_code[] = {
        0x8d, 0x00, 0x03,    // sta $0300
        0x8e, 0x01, 0x03,    // stx $0301
        0x8c, 0x02, 0x03,    // sty $0302
        0x08,                // php
        0x68,                // pla
        0x8d, 0x03, 0x03,    // sta $0303
        0x4c, 0x0e, 0x02,    // jmp $020e
    };

    init_process = palloc();
    if(!init_process){
        panic("palloc");
    }

    memcpy(&init_process->ctx, &ctx, sizeof(Context));

    init_process->page_table[0] = kalloc();
    if(init_process->page_table[0] == FRAME_UNUSED){
        panic("kalloc");
    }

    if(copy_to_user(init_code, 0x0200, sizeof(init_code), init_process->page_table) < 0){
        panic("copy_to_user");
    }

    init_process->state = PROC_STATE_READY;
}