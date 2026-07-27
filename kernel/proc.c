
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
    uint8_t  exit_code;         
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
    p->state = PROC_STATE_NEW;
    p->pid = pid_alloc(); 
    p->ctx.sp = 0xff;
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        p->page_table[i] = FRAME_UNUSED;
    }

    return p;
}

void pfree(Proc* p){
    if(!p) panic("pfree");
    memset(p, 0, sizeof(*p));
    p->state = PROC_STATE_UNUSED;
}

const Context* proc_get_ctx(const Proc* p){
    return &p->ctx;
}

// X is High, A is Low
uint16_t proc_get_ax(const Proc* p){
    uint8_t a;
    uint8_t x;
    uint16_t ax;
    a = (uint8_t)proc_get_ctx(p)->a;
    x = (uint8_t)proc_get_ctx(p)->x;
    ax = ((uint16_t)x << 8) | a;
    return ax;
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
        if (physical_frame == FRAME_UNUSED) {
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
        if (physical_frame == FRAME_UNUSED) {
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

// Save the process's CPU context and page table INTO the Trap Segment "Life Raft"
void kernel_epilogue(void){
    if(current_process->killed != 0 && current_process != init_process){
        current_process->ctx.a = SIGKILL;
        sys_exit();
    }
    memcpy(life_raft, &current_process->ctx,sizeof(Context));
    memcpy(life_raft + 8, current_process->page_table, PAGE_TABLE_SIZE);
}

// Load the process's CPU context and page table FROM the Trap Segment "Life Raft"
void kernel_prologue(void){
    if(current_process->killed != 0 && current_process != init_process){
        current_process->ctx.a = SIGKILL;
        sys_exit();
    }
    memcpy(&current_process->ctx, life_raft, sizeof(Context));
    memcpy(current_process->page_table, life_raft + 8, PAGE_TABLE_SIZE);
}

// global counter to track how deep we are in nested critical sections
static uint8_t interrupt_depth = 0;

// always disable hardware IRQ's
void interrupts_push(void) {

    asm("sei");
    
    interrupt_depth++;
    
    if (interrupt_depth == 0) {
        panic("interrupts_push");
    }
}

// only re-enable hardware interrupts if we have fully exited all nested critical sections
void interrupts_pop(void) {
    if (interrupt_depth == 0) {
        panic("interrupts_pop");
    }

    interrupt_depth--;
    
    if (interrupt_depth == 0) {
        asm("cli");
    }
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

            kernel_epilogue();

            // no return
            return_from_trap(); 
        }

        if (round_robin_index >= MAX_PROC_COUNT) {
            round_robin_index = 0;
        }
    }
}

int sys_wait(void){
    uint8_t i;
    uint8_t res;
    bool has_children = false;
    uint16_t user_exit_code = proc_get_ax(current_process);

    for(i = 0; i < ARRAY_SIZE(proc_table); i++){
        if(proc_table[i].parent == current_process){
            has_children = true;

            if(proc_table[i].state == PROC_STATE_ZOMBIE){
                if(user_exit_code != 0){
                    if(copy_to_user(&proc_table[i].exit_code, user_exit_code, 
                                    sizeof(proc_table[i].exit_code), current_process->page_table) < 0)
                    {
                        current_process->ctx.a = SEGFAULT;
                        sys_exit(); 
                    }
                }
                res = proc_table[i].pid;
                pfree(&proc_table[i]);
                return res; 
            }
        }
    }

    // no children
    if(!has_children){
        return -1;
    }

    // have children, but none are zombie, so we must block
    // a way to block in this no "Trail of Breadcrumbs" stateless kernel
    // rewind the PC by 2.
    current_process->ctx.pc -= 2;

    sleep(current_process);

    // for the compiler...
    return -1; 
}

int sys_exit(void){
    uint8_t segment;
    uint8_t i;

    if(current_process == init_process){
        panic("init exited");
    }

    // TODO: close(decrement reference count of) open files and cwd 

    // free process memory frames 
    for(segment = 0; segment < PAGE_TABLE_SIZE; segment++){
        if(current_process->page_table[segment] != FRAME_UNUSED){
            kfree(current_process->page_table[segment]);
            current_process->page_table[segment] = FRAME_UNUSED;
        }
    }

    wakeup(current_process->parent);

    current_process->exit_code = current_process->ctx.a;

    current_process->state = PROC_STATE_ZOMBIE;

    // re-parent its children
    for(i = 0; i < ARRAY_SIZE(proc_table); i++){
        if(proc_table[i].parent == current_process){
            proc_table[i].parent = init_process;
            if(proc_table[i].state == PROC_STATE_ZOMBIE){
                wakeup(init_process);
            }
        }
    }

    // yield the CPU forever...
    scheduler();

    // for the compiler...
    return -1;
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
    child->exit_code   = current_process->exit_code;
    child->killed  = current_process->killed;
    child->priority = current_process->priority;
    child->ticks   = current_process->ticks;
    child->top     = current_process->top;
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

    // fork magic, returning 0 for the child and child pid for the parent
    proc_set_ax(child, 0);
    return child->pid;
}

void run_init_process(void){

    const char* name = "init";

    Context ctx = {
       0xff,                // SP
       0x00,                // P
       (uint16_t)init_code, // PC
       0x00,                // X
       0x00,                // Y
       0x00                 // A
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

    if(copy_to_user((void*)_INITCODE_LOAD__, (uint16_t)init_code, (uint16_t)_INITCODE_SIZE__, init_process->page_table) < 0){
        panic("copy_to_user");
    }

    memcpy(init_process->name, name, strlen(name));

    init_process->state = PROC_STATE_READY;
}
