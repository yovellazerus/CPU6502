
#include "comman.h"

struct Proc {

    // CPU and memory context
    Context ctx; 
    uint8_t page_table[PAGE_TABLE_SIZE];
    uint8_t kernel_stack_frame;
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

/*
create a new Proc struct with an empty page table, 
new pid, SP set to $ff and a state of PROC_STATE_NEW,
and a kernel stack frame alocted.
*/
Proc* palloc(void){
    Proc* p = 0;
    uint8_t i;
    uint8_t stack_frame;

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

    // give the process a fresh empty kernel stack
    stack_frame = p->kernel_stack_frame = kalloc();
    if(p->kernel_stack_frame == FRAME_UNUSED){
        return NULL;
    }

    // initialize the software kernel stack by writing to it's c_sp zero page register   
    make_kernel_stack(stack_frame);    

    p->state = PROC_STATE_NEW;
    p->pid = pid_alloc(); 
    p->ctx.sp = 0xff;
    p->ctx.ksp = 0xff;
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        p->page_table[i] = FRAME_UNUSED;
    }

    return p;
}

void pfree(Proc* p){
    if(!p) panic("pfree");
    kfree(p->kernel_stack_frame);
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
    a = p->ctx.a;
    x = p->ctx.x;
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

const char* proc_get_name(const Proc* p){
    return p->name;
}

uint16_t proc_get_pid(const Proc* p){
    return p->pid;
}

uint8_t proc_ticks_dec(Proc* p){
    return --p->ticks;
}

void proc_set_state(Proc* p, Proc_State state){
    p->state = state;
}

int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, uint8_t* page_table){
    uint16_t offset;
    uint8_t segment;
    uint8_t physical_frame;
    uint16_t bytes_in_frame;
    uint16_t chunk;
    uint8_t* dst_window;
    uint8_t old_frame;
    uint16_t i;
    uint8_t* src = (uint8_t*)kernel_src;
    
    old_frame = MMIO8(MMU_PAGE_TABLE + 1);

    while (n > 0) {
        
        segment = user_dest >> 12;
        offset = user_dest & 0x0FFF;
        
        physical_frame = page_table[segment];
        if (physical_frame == FRAME_UNUSED) {
            MMIO8(MMU_PAGE_TABLE + 1) = old_frame;
            return -1; // segfault
        }
        
        MMIO8(MMU_PAGE_TABLE + 1) = physical_frame;
        
        bytes_in_frame = 4096 - offset;
        chunk = (n < bytes_in_frame) ? n : bytes_in_frame;

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
    uint8_t segment;
    uint8_t physical_frame;
    uint16_t bytes_in_frame;
    uint16_t chunk;
    uint8_t* src_window;
    uint8_t old_frame;
    uint16_t i;
    uint8_t* dst = (uint8_t*)kernel_dest;
    
    old_frame = MMIO8(MMU_PAGE_TABLE + 1);

    while (n > 0) {
        
        segment = user_src >> 12;
        offset = user_src & 0x0FFF;
        
        physical_frame = page_table[segment];
        if (physical_frame == FRAME_UNUSED) {
            MMIO8(MMU_PAGE_TABLE + 1) = old_frame;
            return -1; // segfault
        }
        
        MMIO8(MMU_PAGE_TABLE + 1) = physical_frame;
        
        bytes_in_frame = 4096 - offset;
        chunk = (n < bytes_in_frame) ? n : bytes_in_frame;

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

void kernel_prologue(void){
    if(current_process->killed != 0 && current_process != init_process){
        printk("\"%s\" [%d] terminated by a different process\n", proc_get_name(current_process), proc_get_pid(current_process));
        current_process->ctx.a = SIGKILL;
        sys_exit();
    }

    // load the process's CPU context and page table FROM the Trap Segment "Life Raft"
    // NOTE: the kernel_stack_frame and the kernel hardware stack pointer (KSP) of the process,
    // are installed by the _nmi_handler() and _irq_handler() assembly trampoline.s routines
    memcpy(&current_process->ctx, life_raft, sizeof(Context));

    memcpy(current_process->page_table, life_raft + 8, sizeof(current_process->page_table));

    current_process->kernel_stack_frame = kernel_page_table[0];
}

void kernel_epilogue(void){
    if(current_process->killed != 0 && current_process != init_process){
        printk("\"%s\" [%d] terminated by a different process\n", proc_get_name(current_process), proc_get_pid(current_process));
        current_process->ctx.a = SIGKILL;
        sys_exit();
    }
    
    // save the process's CPU context and page table INTO the Trap Segment "Life Raft"
    // NOTE: not installing the kernel stack frame, it is just saving it to the trampoline!
    // so it can be loaded back to the CPU and to the MMU in the _nmi_handler() and _irq_handler()

    memcpy(life_raft, &current_process->ctx, sizeof(Context));
    
    memcpy(life_raft + 8, current_process->page_table, sizeof(current_process->page_table));
    
    kernel_page_table[0] = current_process->kernel_stack_frame;
}

// global counter to track how deep we are in nested critical sections
static uint8_t interrupt_depth = 0;

// always disable hardware IRQ's
void interrupts_push(void) {

    __asm__("sei");
    
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
        __asm__("cli");
    }
}

// will yield the cpu
void sleep(void* channel){
    current_process->channel = channel;
    current_process->state = PROC_STATE_SLEEPING;
    scheduler();
    current_process->channel = NULL;
}

void wakeup(void* channel){
    uint8_t i;
    for (i = 0; i < ARRAY_SIZE(proc_table); i++) {
        if (proc_table[i].state == PROC_STATE_SLEEPING && proc_table[i].channel == channel) {
            proc_table[i].state = PROC_STATE_READY;
        }
    }
}

void scheduler(void) {
    static uint8_t round_robin_index = 0;
    Proc* p;
    uint16_t i;
    bool is_new;
    Proc* old = current_process;
    
    while (1) {
        for (i = 0; i < MAX_PROC_COUNT; i++) {
            p = &proc_table[round_robin_index++];
            if (round_robin_index >= MAX_PROC_COUNT) {
                round_robin_index = 0;
            }
            
            if (p->state == PROC_STATE_READY || p->state == PROC_STATE_NEW) {
                
                is_new = (p->state == PROC_STATE_NEW);
                p->state = PROC_STATE_RUNING;
                current_process = p;
                p->ticks = QUANTUM; 

                if (old == NULL) {
                    //  "init" first run, so no previous process to save
                    kernel_epilogue();
                    return_from_trap();

                } else if (is_new) {
                    // p is a new process created by sys_fork()
                    first_context_switch(old, p);
                    return; 

                } else {
                    context_switch(old, p);
                    return;
                }
            }
        }

        // idle state. 
        // no process is READY to run,
        // so we must enable interrupt's to avoid deadlock 
        // a "WAI" instruction would have been nice...
        __asm__("cli"); 
        for(i = 0; i < CYCLES; i++) __asm__("nop");
        __asm__("sei"); 
    }
}

int sys_kill(void){
    uint16_t pid;
    uint8_t i;

    pid = proc_get_ax(current_process);

    // cant kill "init"
    if(pid == 1) return -1;

    for(i = 0; i < ARRAY_SIZE(proc_table); i++){
        if(proc_table[i].pid == pid){
            proc_table[i].killed = 1;
            // if the victim is asleep wake it up, 
            // so it can return to user space and the kernel will call sys_exit() on it.
            if(proc_table[i].state == PROC_STATE_SLEEPING){
                proc_table[i].state = PROC_STATE_READY;
            }
            // success
            return 0;
        }
    }
    // there is no such process
    return -1;
}

int sys_wait(void){
    uint8_t i;
    uint8_t res;
    bool has_children = false;
    uint16_t user_exit_code = proc_get_ax(current_process);

    while(true){
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

        // there are children, but they are all not zombies, 
        // when one of them exits, it will wake this process up and it will return here to the while loop
        // to reap the new zombie child process
        sleep(current_process);
    }
}

int sys_exit(void){
    uint8_t segment;
    uint8_t i;

    if(current_process == init_process){
        panic("init exited");
    }

    // TODO: close (decrement reference count of) open files and cwd 

    // free process memory frames, not including the kernel stack frame, that will be freed by pfree()
    for(segment = 0; segment < PAGE_TABLE_SIZE; segment++){
        if(current_process->page_table[segment] != FRAME_UNUSED){
            kfree(current_process->page_table[segment]);
            current_process->page_table[segment] = FRAME_UNUSED;
        }
    }

    wakeup(current_process->parent);

    current_process->exit_code = current_process->ctx.a;

    current_process->state = PROC_STATE_ZOMBIE;

    // re-parent its children to "init" and if a child is zombie it will wake up "init" so "init" can reap it
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

    panic("zombie exit");

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
        printk("process pool exhausted\n");
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

            // no more memory frames, clean the new process page table, free the new Proc struct and return error code -1
            if (child_frame == FRAME_UNUSED) {
                do{
                    if(child->page_table[segment] != FRAME_UNUSED){
                        kfree(child->page_table[segment]);
                        child->page_table[segment] = FRAME_UNUSED;
                    }
                    segment--;
                }
                while(segment > 0);
                // restore the kernel's memory space
                MMIO8(MMU_PAGE_TABLE + 1) = old_window1;
                MMIO8(MMU_PAGE_TABLE + 2) = old_window2;
                pfree(child);
                printk("frame pool exhausted in fork()"); 
                return -1;
            }

            child->page_table[segment] = child_frame;

            // map the parent's and child frame's to the copy window's
            MMIO8(MMU_PAGE_TABLE + 1) = parent_frame;
            MMIO8(MMU_PAGE_TABLE + 2) = child_frame;

            // copy the frame
            memcpy((void*)WINDOW2, (void*)WINDOW1, 4096);
        }
    }

    // copy the kernel stack
    MMIO8(MMU_PAGE_TABLE + 1) = current_process->kernel_stack_frame;
    MMIO8(MMU_PAGE_TABLE + 2) = child->kernel_stack_frame;
    memcpy((void*)WINDOW2, (void*)WINDOW1, 4096);

    // restore the kernel's memory space
    MMIO8(MMU_PAGE_TABLE + 1) = old_window1;
    MMIO8(MMU_PAGE_TABLE + 2) = old_window2;

    // copy the context, including the kernel hardware stack pointer
    memcpy(&child->ctx, &current_process->ctx, sizeof(Context));

    // TODO: this should be deeper... using reference count
    memcpy(child->fd_table, current_process->fd_table, sizeof(child->fd_table));
    child->cwd_inode = current_process->cwd_inode;

    // copy the name
    memcpy(child->name, current_process->name, MAX_PROC_NAME);

    // construct the process tree
    child->parent  = current_process;

    // PROC_STATE_NEW to indicate to the scheduler() that it is its first run
    child->state = PROC_STATE_NEW;

    // fork magic, returning 0 for the child and the child pid for the parent
    proc_set_ax(child, 0);
    return child->pid;
}

void run_init_process(void){

    const char* name = "init";

    init_process = palloc();
    if(!init_process){
        panic("palloc in run_init_process");
    }

    init_process->page_table[0] = kalloc();
    if(init_process->page_table[0] == FRAME_UNUSED){
        panic("kalloc in run_init_process");
    }

    init_process->ctx.pc = (uint16_t)init_code;

    if(copy_to_user((void*)_INITCODE_LOAD__, (uint16_t)init_code, (uint16_t)_INITCODE_SIZE__, init_process->page_table) < 0){
        panic("copy_to_user in run_init_process");
    }

    memcpy(init_process->name, name, strlen(name));
}
