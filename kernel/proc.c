
#include "comman.h"

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
    void* channel;       
    uint8_t  killed;      

    // file system
    uint16_t cwd_inode;     
    uint8_t  fd_table[MAX_FILES_PER_PROC];          

    // debug 
    char name[MAX_PROC_NAME];
};

Proc proc_table[MAX_PROC_COUNT];
Proc* init_process;
Proc* current_process;

static int pid_alloc(void){
    static uint16_t next_pid = 1;
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
}

// create a new Proc struct with empty page table, new pid and SP in a USED state 
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

const Context* proc_get_ctx(const Proc* p){
    return &p->ctx;
}

void proc_set_ax(Proc* p, uint16_t ax){
    p->ctx.a = (uint8_t)(ax & 0x00ff);
    p->ctx.x = (uint8_t)((ax & 0xff00) >> 8);
}

const uint8_t* proc_get_page_table(const Proc* p){
    return p->page_table;
}

uint8_t proc_get_ticks(const Proc* p){
    return p->ticks;
}

uint16_t proc_get_pid(const Proc* p){
    return p->pid;
}

uint8_t proc_ticks_dec(Proc* p){
    uint8_t ticks = p->ticks;
    p->ticks--;
    return ticks;
}

void proc_set_state(Proc* p, Proc_State state){
    p->state = state;
}

int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, uint8_t* page_table){
    uint16_t offset;
    uint8_t seg;
    uint8_t physical_frame;
    uint16_t bytes_in_page;
    uint16_t chunk;
    uint8_t* dst_window;
    uint8_t old_frame;
    uint16_t i;
    uint8_t* src = (uint8_t*)kernel_src;
    
    while (n > 0) {
        
        seg = user_dest >> 12;
        offset = user_dest & 0x0FFF;
        
        physical_frame = page_table[seg];
        if (physical_frame == 0) {
            return -1; // segfault
        }
        
        old_frame = MMIO8(MMU_PAGE_TABLE + 1);
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

    MMIO8(MMU_PAGE_TABLE + 1) = old_frame;

    return 0; // success
}

int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, const uint8_t* page_table){
    uint16_t offset;
    uint8_t seg;
    uint8_t physical_frame;
    uint16_t bytes_in_page;
    uint16_t chunk;
    uint8_t* src_window;
    uint8_t old_frame;
    uint16_t i;
    uint8_t* dst = (uint8_t*)kernel_dest;
    
    while (n > 0) {
        
        seg = user_src >> 12;
        offset = user_src & 0x0FFF;
        
        physical_frame = page_table[seg];
        if (physical_frame == 0) {
            return -1; // segfault
        }
        
        old_frame = MMIO8(MMU_PAGE_TABLE + 1);
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

    MMIO8(MMU_PAGE_TABLE + 1) = old_frame;

    return 0; // success
}

// Copy the process's CPU context and page table into the Trap Segment "Life Raft"
void copy_to_life_raft(Proc* p){
    memcpy(life_raft, &p->ctx,sizeof(Context));
    memcpy(life_raft + 8, p->page_table, PAGE_TABLE_SIZE);
}

void copy_from_life_raft(Proc* p){
    memcpy(&p->ctx, life_raft, sizeof(Context));
    memcpy(p->page_table, life_raft + 8, PAGE_TABLE_SIZE);
}

void interrupts_on(void){
    asm("cli");
}

void interrupts_off(void){
    asm("sei");
}

// will yield the cpu
void sleep(void* channel){
    current_process->channel = channel;
    current_process->state = PROC_STATE_SLEEPING;
    scheduler();
}

void wakeup(void* channel){
    uint8_t i;
    for (i = 0; i < ARRAY_SIZE(proc_table); i++) {
        if (proc_table[i].state == PROC_STATE_SLEEPING && proc_table[i].channel == channel) {
            proc_table[i].state = PROC_STATE_READY;
            proc_table[i].channel = NULL;
        }
    }
    
}

void scheduler(void) {
    
    static uint8_t round_robin_index = 0;
    Proc* p;

    while (1) {
        
        // TODO: to avoid deadlock, need to enable interrupts

        p = &proc_table[round_robin_index++];
        
        if (p->state == PROC_STATE_READY) {
            
            p->state = PROC_STATE_RUNING;
            current_process = p;
            
            p->ticks = QUANTUM; 

            copy_to_life_raft(current_process);

            // no return
            return_from_trap(); 
        }

        if (round_robin_index >= MAX_PROC_COUNT) {
            round_robin_index = 0;
        }
    }
}

int sys_fork(void){
    Proc* child;
    uint8_t segment;
    uint8_t parent_frame;
    uint8_t child_frame;
    uint8_t old_window1;
    uint8_t old_window2;

    child = palloc();
    if(!child){
        printk("palloc\n");
        return -1;
    }

    // equal primitives
    child->channel = current_process->channel;
    child->gid     = current_process->gid;
    child->ecode   = current_process->ecode;
    child->killed  = current_process->killed;
    child->priority = current_process->priority;
    child->ticks   = current_process->ticks;
    child->top     = child->top;
    child->uid     = current_process->uid;
    child->ticks   = current_process->ticks;
    child->state   = current_process->state;

    // for restoration later
    old_window1 = MMIO8(MMU_PAGE_TABLE + 1);
    old_window2 = MMIO8(MMU_PAGE_TABLE + 2);
    // clone the memory space
    for (segment = 0; segment < PAGE_TABLE_SIZE; segment++) {
        parent_frame = current_process->page_table[segment];
        
        if (parent_frame != FRAME_UNUSED) {
            
            child_frame = kalloc();
            if (child_frame == FRAME_UNUSED) {
                // TODO: destroy all and retrun -1
                panic("out of memory in fork"); 
            }
            child->page_table[segment] = child_frame;

            // map the parent's and child frame's to the copy window's
            MMIO8(MMU_PAGE_TABLE + 1) = parent_frame;
            MMIO8(MMU_PAGE_TABLE + 2) = child_frame;

            // copy the frame
            memcpy((void*)WINDOW2, (void*)WINDOW1, 4096);
        }
    }
    // restore the kernel's memory space
    MMIO8(MMU_PAGE_TABLE + 1) = old_window1;
    MMIO8(MMU_PAGE_TABLE + 2) = old_window2;

    // copy the context
    memcpy(&child->ctx, &current_process->ctx, sizeof(Context));

    // TODO: this should be deeper... using reference count
    memcpy(child->fd_table, current_process->fd_table, sizeof(child->fd_table));
    child->cwd_inode = current_process->cwd_inode;

    // name
    memcpy(child->name, current_process->name, MAX_PROC_NAME);

    // process tree
    child->parent  = current_process;

    // wake up the child
    child->state = PROC_STATE_READY;

    // fork magic, returning 0 for the child and child pid for the perent
    proc_set_ax(child, 0);
    return child->pid;
}

void run_init_process(void){

    const char* name = "init";

    Context ctx = {
       0xff,   // SP
       0x00,   // P
       0x0200, // PC
       0x00,   // X
       0x00,   // Y
       0x00    // A
    };

    // for testing, not the real init code
    /*
    .org $0200
    _start:
      ldy #70
      brk
      nop
      cmp #0
      bne perent
      beq child
    error:
      jmp *
    
    
    child:
      ldy #87
      lda #<child_arg
      ldx #>child_arg
      brk
      nop
      jmp child
    
    
    perent:
      ldy #87
      lda #<perent_arg
      ldx #>perent_arg
      brk
      nop
      jmp perent
    
    child_arg:
      .word 7
      .word child_msg
    child_msg:
      .byte "child"
      .byte $0a
      .byte 0
    
    perent_arg:
      .word 8
      .word perent_msg
    perent_msg:
      .byte "perent"
      .byte $0a
      .byte 0
    */
    uint8_t init_code[]  = {
        0xA0, 0x46, 0x00, 0xEA, 0xC9, 0x00, 0xD0, 0x10,
        0xF0, 0x03, 0x4C, 0x0A, 0x02, 0xA0, 0x57, 0xA9,
        0x23, 0xA2, 0x02, 0x00, 0xEA, 0x4C, 0x0D, 0x02,
        0xA0, 0x57, 0xA9, 0x2E, 0xA2, 0x02, 0x00, 0xEA,
        0x4C, 0x18, 0x02, 0x07, 0x00, 0x27, 0x02, 0x63,
        0x68, 0x69, 0x6C, 0x64, 0x0A, 0x00, 0x08, 0x00,
        0x32, 0x02, 0x70, 0x65, 0x72, 0x65, 0x6E, 0x74,
        0x0A, 0x00,
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

    memcpy(init_process->name, name, strlen(name));

    init_process->state = PROC_STATE_READY;
}

// for debug
void run_extra_process(void){

    const char name[] = "Second";

    Context ctx = {
       0xff,   // SP
       0x00,   // P
       0x0200, // PC
       0x00,   // X
       0x00,   // Y
       0x00    // A
    };

    // for testing, not the real init code
    uint8_t code[] = {

        0xa0, SYS_WRITE,        // ldy SYS_WRITE
        0xa9, 0x14,             // lda #<arg 
        0xa2, 0x02,             // ldx #>arg
        0x00,                   // brk
        0xea,                   // nop

        0x8d, 0x00, 0x03,       // sta $0300
        0x8e, 0x01, 0x03,       // stx $0301
        0x8c, 0x02, 0x03,       // sty $0302

        0x4c, 0x00, 0x02,       // jmp _start

        0x12, 0x00,             // arg.size
        0x18, 0x02,             // arg.buffer
        'S', 'e', 'c', 'o', 'n', 'd', ' ', 'p', 'r', 'o', 'c', ':', ' ', 'h', 'i', '?', '\n', '\0'
    };

    Proc* p;

    p = palloc();
    if(!p){
        panic("palloc");
    }

    memcpy(&p->ctx, &ctx, sizeof(Context));

    p->page_table[0] = kalloc();
    if(p->page_table[0] == FRAME_UNUSED){
        panic("kalloc");
    }

    if(copy_to_user(code, 0x0200, sizeof(code), p->page_table) < 0){
        panic("copy_to_user");
    }

    memcpy(p->name, name, strlen(name));

    p->state = PROC_STATE_READY;
}

void run_extra_process2(void){

    const char name[] = "Third";

    Context ctx = {
       0xff,   // SP
       0x00,   // P
       0x0200, // PC
       0x00,   // X
       0x00,   // Y
       0x00    // A
    };

    // for testing, not the real init code
    uint8_t code[] = {

        0xa0, SYS_WRITE,        // ldy SYS_WRITE
        0xa9, 0x14,             // lda #<arg 
        0xa2, 0x02,             // ldx #>arg
        0x00,                   // brk
        0xea,                   // nop

        0x8d, 0x00, 0x03,       // sta $0300
        0x8e, 0x01, 0x03,       // stx $0301
        0x8c, 0x02, 0x03,       // sty $0302

        0x4c, 0x00, 0x02,       // jmp _start

        0x0e, 0x00,             // arg.size
        0x18, 0x02,             // arg.buffer
        'T', 'h', 'i', 'r', 'd', ' ', 'p', 'r', 'o', 'c','e', 's', 's', '\n', '\0'
    };

    Proc* p;

    p = palloc();
    if(!p){
        panic("palloc");
    }

    memcpy(&p->ctx, &ctx, sizeof(Context));

    p->page_table[0] = kalloc();
    if(p->page_table[0] == FRAME_UNUSED){
        panic("kalloc");
    }

    if(copy_to_user(code, 0x0200, sizeof(code), p->page_table) < 0){
        panic("copy_to_user");
    }

    memcpy(p->name, name, strlen(name));

    p->state = PROC_STATE_READY;
}

