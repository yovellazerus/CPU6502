
#include "comman.h"

#define PCB_OFFSET 0x200
#define MAP_PCB(proc, old_frame) (PCB*)((uint16_t)mmu_map_window(2, proc->kernel_low_memory[0], &old_frame) + PCB_OFFSET)
#define UNMAP_PCB(old_frame)     mmu_unmap_window(2, old_frame)  

// in virtual static memory, try to keep as thin as possible!
// WARNING: order is importent for trampoline.s
struct Proc {
    uint8_t ksp;                    // for trampoline context switch functions 
    uint8_t kernel_low_memory[3];   // for the MMU map/unmap functions, and for trampoline and tracing
    Proc_State state;               // for fast scheduler and process syscalls actions
    void* channel;                  // in here so interrupts will not map dynamic windows
};

// process control block (cached to the kernel stack frame so, can be as large as needed)
struct PCB {

    // CPU and memory context
    uint8_t page_table[PAGE_TABLE_SIZE];
    Context ctx;
    uint16_t top;
     
    // scheduler
    uint16_t pid;                   
    uint8_t  uid;           
    uint8_t  gid;           
    uint8_t  exit_code;         
    uint8_t  priority;      
    uint8_t  ticks;   

    // inter process communication
    Proc* parent;
    uint16_t  killed;

    // file system
    uint16_t cwd_inode;     
    File* open_files[MAX_FILES_PER_PROC];          

    // debug 
    char name[MAX_PROC_NAME];
};

Proc  proc_table[MAX_PROC_COUNT];
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
create a new Proc struct and a cached PCB struct in its kernel stack frame with an empty page table, 
new pid, SP set to $ff and a state of PROC_STATE_BUILDING,
*/
Proc* palloc(void){
    Proc* p = 0;
    uint16_t i;
    uint8_t stack_frame;
    uint8_t old_frame;
    PCB* pcb;

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
    stack_frame = p->kernel_low_memory[0] = kalloc();
    if(p->kernel_low_memory[0] == FRAME_UNUSED) return NULL;
    make_kernel_stack(stack_frame); 

    // initialize the Proc fields
    p->kernel_low_memory[1] = 1; 
    p->kernel_low_memory[2] = 2;
    p->state = PROC_STATE_BUILDING;
    p->ksp = 0xff;
    p->channel = NULL;

    // initialize the cached PCB struct
    pcb = MAP_PCB(p, old_frame);
    memset(pcb, 0, sizeof(PCB));
    pcb->pid = pid_alloc();
    pcb->ctx.sp = 0xff;
    pcb->ctx.ksp = 0xff;
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        pcb->page_table[i] = FRAME_UNUSED;
    }
    UNMAP_PCB(old_frame);
    
    return p;
}

void pfree(Proc* p){
    if(!p) panic("pfree");
    kfree(p->kernel_low_memory[0]);
    memset(p, 0, sizeof(*p));
    p->state = PROC_STATE_UNUSED;
}

// X is High, A is Low
uint16_t proc_get_ax(const Proc* p){
    uint8_t a;
    uint8_t x;
    uint16_t ax;
    uint8_t old_frame;
    PCB* pcb;

    pcb = MAP_PCB(p, old_frame);
    a = pcb->ctx.a;
    x = pcb->ctx.x;
    ax = ((uint16_t)x << 8) | a;
    UNMAP_PCB(old_frame);
    return ax;
}

uint8_t proc_get_y(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint8_t y;
    pcb = MAP_PCB(p, old_frame);
    y = pcb->ctx.y;
    UNMAP_PCB(old_frame);
    return y;
}

void proc_set_ax(Proc* p, uint16_t ax){
    uint8_t old_frame;
    PCB* pcb;

    pcb = MAP_PCB(p, old_frame);
    pcb->ctx.a = (uint8_t)(ax & 0x00ff);
    pcb->ctx.x = (uint8_t)((ax & 0xff00) >> 8);
    UNMAP_PCB(old_frame);
}

void proc_set_a(Proc* p, uint8_t a){
    uint8_t old_frame;
    PCB* pcb;

    pcb = MAP_PCB(p, old_frame);
    pcb->ctx.a = a;
    UNMAP_PCB(old_frame);
}

uint8_t* proc_get_kernel_low_memory(Proc* p){
    return (uint8_t*)p->kernel_low_memory;
}

void proc_get_ctx(const Proc* p, Context* ctx){
    uint8_t old_frame;
    PCB* pcb;
    pcb = MAP_PCB(p, old_frame);
    *ctx = pcb->ctx;
    UNMAP_PCB(old_frame);
}

void proc_set_ctx(Proc* p, Context* ctx){
    uint8_t old_frame;
    PCB* pcb;
    pcb = MAP_PCB(p, old_frame);
    pcb->ctx = *ctx;
    UNMAP_PCB(old_frame);
}

Proc* proc_get_parent(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    Proc* parent;
    pcb = MAP_PCB(p, old_frame);
    parent = pcb->parent;
    UNMAP_PCB(old_frame);
    return parent;
}

void proc_set_parent(Proc* p, Proc* parent){
    uint8_t old_frame;
    PCB* pcb;
    pcb = MAP_PCB(p, old_frame);
    pcb->parent = parent;
    UNMAP_PCB(old_frame);
}

uint8_t proc_get_exit_code(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint8_t exit_code;
    pcb = MAP_PCB(p, old_frame);
    exit_code = pcb->exit_code;
    UNMAP_PCB(old_frame);
    return exit_code;
}

void proc_get_page_table(const Proc* p, uint8_t page_table[PAGE_TABLE_SIZE]){
    uint8_t old_frame;
    PCB* pcb;

    pcb = MAP_PCB(p, old_frame);
    memcpy(page_table, pcb->page_table, PAGE_TABLE_SIZE);
    UNMAP_PCB(old_frame);
}

void proc_set_page_table(Proc* p, uint8_t page_table[PAGE_TABLE_SIZE]){
    uint8_t old_frame;
    PCB* pcb;

    pcb = MAP_PCB(p, old_frame);
    memcpy(pcb->page_table, page_table, PAGE_TABLE_SIZE);
    UNMAP_PCB(old_frame);
}

uint8_t proc_get_ticks(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint8_t ticks;
    pcb = MAP_PCB(p, old_frame);
    ticks = pcb->ticks;
    UNMAP_PCB(old_frame);
    return ticks;
}

void proc_set_ticks(Proc* p, uint8_t ticks){
    uint8_t old_frame;
    PCB* pcb;
    pcb = MAP_PCB(p, old_frame);
    pcb->ticks = ticks;
    UNMAP_PCB(old_frame);
}

uint16_t proc_get_top(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint16_t top;
    pcb = MAP_PCB(p, old_frame);
    top = pcb->top;
    UNMAP_PCB(old_frame);
    return top;
}

void proc_set_top(Proc* p, uint16_t top){
    uint8_t old_frame;
    PCB* pcb;
    pcb = MAP_PCB(p, old_frame);
    pcb->top = top;
    UNMAP_PCB(old_frame);
}

void proc_set_killed(Proc* p, uint16_t killed){
    uint8_t old_frame;
    PCB* pcb;
    pcb = MAP_PCB(p, old_frame);
    pcb->killed = killed;
    UNMAP_PCB(old_frame);
}

uint16_t proc_get_killed(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint16_t killed;
    pcb = MAP_PCB(p, old_frame);
    killed = pcb->killed;
    UNMAP_PCB(old_frame);
    return killed;
}

uint8_t proc_get_uid(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint8_t uid;
    pcb = MAP_PCB(p, old_frame);
    uid = pcb->uid;
    UNMAP_PCB(old_frame);
    return uid;
}

File* proc_get_file(const Proc* p, int fd){
    uint8_t old_frame;
    PCB* pcb;
    File* file;

    if(fd < 0 || fd >= MAX_FILES_PER_PROC) return NULL;
    
    pcb = MAP_PCB(p, old_frame);
    file = pcb->open_files[fd];
    UNMAP_PCB(old_frame);
    return file;
}

void proc_get_name(const Proc* p, char name[MAX_PROC_NAME]){
    uint8_t old_frame;
    PCB* pcb;

    pcb = MAP_PCB(p, old_frame);
    memcpy(name, pcb->name, MAX_PROC_NAME);
    UNMAP_PCB(old_frame);
}

uint16_t proc_get_pid(const Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint16_t pid;
    pcb = MAP_PCB(p, old_frame);
    pid = pcb->pid;
    UNMAP_PCB(old_frame);
    return pid;
}

uint8_t proc_ticks_dec(Proc* p){
    uint8_t old_frame;
    PCB* pcb;
    uint16_t ticks;
    pcb = MAP_PCB(p, old_frame);
    ticks = --pcb->ticks;
    UNMAP_PCB(old_frame);
    return ticks;
}

void proc_set_state(Proc* p, Proc_State state){
    p->state = state;
}

int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, Proc* p){
    uint16_t offset;
    uint8_t segment;
    uint8_t physical_frame;
    uint16_t bytes_in_frame;
    uint16_t chunk;
    uint8_t* dst_window;
    uint8_t old_frame;
    uint16_t i;
    uint8_t* src = (uint8_t*)kernel_src;
    uint8_t page_table[PAGE_TABLE_SIZE];

    if(user_dest >= proc_get_top(p)){
        return -1;
    }

    proc_get_page_table(p, page_table);
    
    while(n > 0){
        
        segment = user_dest >> 12;
        offset = user_dest & 0x0FFF;
        
        physical_frame = page_table[segment];
        if (physical_frame == FRAME_UNUSED){
            return -1;
        }
        
        dst_window = mmu_map_window(1, physical_frame, &old_frame);
        
        bytes_in_frame = 4096 - offset;
        chunk = (n < bytes_in_frame) ? n : bytes_in_frame;

        dst_window += offset;
        
        for (i = 0; i < chunk; i++){
            dst_window[i] = *src++;
        }

        mmu_unmap_window(1, old_frame);

        n -= chunk;
        user_dest += chunk;
    }

    return 0; // success
}

int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, Proc* p){
    uint16_t offset;
    uint8_t segment;
    uint8_t physical_frame;
    uint16_t bytes_in_frame;
    uint16_t chunk;
    uint8_t* src_window;
    uint8_t old_frame;
    uint16_t i;
    uint8_t* dst = (uint8_t*)kernel_dest;
    uint8_t page_table[PAGE_TABLE_SIZE];

    if(user_src >= proc_get_top(p)){
        return -1;
    }

    proc_get_page_table(p, page_table);
    
    while(n > 0){
        
        segment = user_src >> 12;
        offset = user_src & 0x0FFF;
        
        physical_frame = page_table[segment];
        if (physical_frame == FRAME_UNUSED){
            return -1;
        }

        src_window = mmu_map_window(1, physical_frame, &old_frame);
        
        bytes_in_frame = 4096 - offset;
        chunk = (n < bytes_in_frame) ? n : bytes_in_frame;

        src_window += offset;
        
        for (i = 0; i < chunk; i++){
            *dst++ = src_window[i];
        }
        
        mmu_unmap_window(1, old_frame);

        n -= chunk;
        user_src += chunk;
    }

    return 0; // success
}

// WARNING: must be called with interrupts OFF
// the caller must disable interrupts BEFORE checking their sleep condition 
// to prevent "Lost Wakeup" race conditions
void sleep(void* channel){
    current_process->channel = channel;
    current_process->state = PROC_STATE_SLEEPING;
    scheduler();
    current_process->channel = NULL;
}

// WARNING: must be called with interrupts OFF
// modifies the global proc_table, relies on the caller (or the hardware 
// interrupt vector) to hold the lock
void wakeup(void* channel){
    uint16_t i;
    for (i = 0; i < ARRAY_SIZE(proc_table); i++) {
        if (proc_table[i].state == PROC_STATE_SLEEPING && proc_table[i].channel == channel) {
            proc_table[i].state = PROC_STATE_READY;
        }
    }
}

// WARNING: must be called with interrupts OFF
// protects the proc_table scan from hardware interrupts that might call wakeup()
// ensures _context_switch pushes I=1 (interrupts disabled) to the stack, 
// guaranteeing the process safely wakes back up with interrupts still OFF
// yields the CPU to the next READY process.
void scheduler(void) {
    static uint16_t round_robin_index = 0;
    Proc* p;
    uint16_t i;
    bool is_new;
    Proc* old = current_process;

    while(true){

        for(i = 0; i < MAX_PROC_COUNT; i++){
            p = &proc_table[round_robin_index++];

            if(round_robin_index >= MAX_PROC_COUNT){
                round_robin_index = 0;
            }

            if(p->state == PROC_STATE_READY || p->state == PROC_STATE_NEW){

                is_new = (p->state == PROC_STATE_NEW);
                p->state = PROC_STATE_RUNING;
                current_process = p;
                proc_set_ticks(p, QUANTUM); 

                if(old == NULL){
                    //  "init" first run, so no previous process to save
                    kernel_epilogue();
                    return_from_trap();
                } 

                else if(is_new){
                    // p is a new process created by sys_fork()
                    first_context_switch(old, p);
                    return; 
                } 

                else{
                    context_switch(old, p);
                    return;
                }
            }
        }

        // idle state. 
        // no process is READY to run,
        // so we must enable interrupt's to avoid deadlock 
        // a "WAI" instruction would have been nice...
        INTER_ON(); 
        for(i = 0; i < CYCLES; i++) { /* wait */ }
        INTER_OFF();
    }
}

int sys_getpid(void){
    return proc_get_pid(current_process);
}

int sys_sleep(void){
    SyscallArg syscall_arg;
    uint32_t ticks;
    uint32_t total;
    uint16_t ax = proc_get_ax(current_process);

    if(!syscall_populate_argument(&syscall_arg)){
        LOG();
        return -1;
    }

    ticks = syscall_arg.sleep.ticks;

    // modifies a the systicks and the next_wakeup_call that are accessible to interrupts therefore, must be locked
    INTER_OFF();
    total = ticks + systicks;
    // check for overflow, if a the sum of two unsigned integers is SMALLER than one of the them, an overflow occurred
    if (total < systicks) {
        total = 0xFFFFFFFF; // cap it to the maximum possible wait time
    }

    while(true){
        
        if(systicks >= total){
            break;
        }
        // updated the timer next_wakeup_call variable that signal the timer to call wakeup() 
        if(total < next_wakeup_call){
            next_wakeup_call = total;
        }
        
        // time hasn't run out yet
        sleep((void*)&systicks); 
    }
    INTER_ON();

    return 0;

}

int sys_sbrk(void) {

    uint8_t old_segment;
    uint8_t new_segment;
    uint8_t segment;
    uint8_t frame;
    uint16_t new_top;
    uint8_t page_table[PAGE_TABLE_SIZE];

    // can be negative
    int16_t increment = (int16_t)proc_get_ax(current_process);
    
    // we will return this to the user on success
    uint16_t old_top = proc_get_top(current_process);
    
    int32_t overflow_check = old_top + increment;

    proc_get_page_table(current_process, page_table);

    // user is request exceeds the maximum virtual address range
    if (overflow_check >= 0x10000 || overflow_check < 0) { 
        return -1; 
    }

    new_top = (uint16_t)overflow_check;

    // converting form bytes to segment indexes
    // calculate the highest segment for the old top and new top
    // subtract 1 to account for a 4KB page boundary
    old_segment = (old_top == 0) ? 0 : (old_top - 1) >> 12;
    new_segment = (new_top == 0) ? 0 : (new_top - 1) >> 12;

    // growing the heap
    if (increment > 0) {
        
        // allocate the missing frames between the old segment and the new segment
        for (segment = old_segment; segment <= new_segment; segment++) {
            if (page_table[segment] == FRAME_UNUSED) {
                frame = kalloc();
                // no more memory
                if (frame == FRAME_UNUSED) {
                    // roll back and free the new allocated frames and return exit code of -1
                    for(segment; segment > old_segment; segment--){
                        if (page_table[segment] != FRAME_UNUSED) {
                            kfree(page_table[segment]);
                            page_table[segment] = FRAME_UNUSED;
                        }
                    }
                    return -1; 
                }
                
                page_table[segment] = frame;
            }
        }
    } 

    // shrinking the heap
    else if (increment < 0) {

        for (segment = old_segment; segment > new_segment; segment--) {
            if (page_table[segment] != FRAME_UNUSED) {
                kfree(page_table[segment]);
                page_table[segment] = FRAME_UNUSED;
            }
        }
    }

    // commit to the new memory map
    proc_set_page_table(current_process, page_table);

    // commit to the new top
    proc_set_top(current_process, new_top);

    return old_top;
}

int sys_kill(void){
    uint16_t pid;
    uint16_t i;
    Proc* p;

    pid = proc_get_ax(current_process);

    // cannot kill "init" and pid 0 is invalid
    if(pid <= 1) return -1;

    // interrupt can modify the proc state, so must be locked
    INTER_OFF();
    for(i = 0; i < ARRAY_SIZE(proc_table); i++){
        p = &proc_table[i];
    
        if(p->state != PROC_STATE_UNUSED && 
           p->state != PROC_STATE_ZOMBIE && 
           proc_get_pid(p) == pid && 
           proc_get_uid(p) == proc_get_uid(current_process)
        ) 
        {
            // the killed field store the pid of the process that killed this one
            proc_set_killed(p, proc_get_pid(current_process));
            
            // if the victim is asleep, wake it up, so the kernel can kill it
            if(p->state == PROC_STATE_SLEEPING){
                p->state = PROC_STATE_READY;
            }
            INTER_ON();
            return 0; // success
        }
    }
    INTER_ON();
    
    // there is no such active process
    return -1;
}

int sys_wait(void){
    uint16_t i;
    uint8_t res;
    bool has_children = false;
    uint16_t user_exit_code = proc_get_ax(current_process);

    INTER_OFF();
    while(true){
        for(i = 0; i < MAX_PROC_COUNT; i++){
            if(proc_table[i].state != PROC_STATE_UNUSED && proc_get_parent(&proc_table[i]) == current_process){
                has_children = true;

                if(proc_table[i].state == PROC_STATE_ZOMBIE){
                    if(user_exit_code != 0){
                        uint8_t child_exit = proc_get_exit_code(&proc_table[i]);
                        if(copy_to_user(&child_exit, user_exit_code, sizeof(child_exit), current_process) < 0){
                            proc_set_ax(current_process, SEGFAULT);
                            sys_exit(); 
                        }
                    }
                    res = proc_get_pid(&proc_table[i]);
                    pfree(&proc_table[i]);
                    INTER_ON();
                    return res; 
                }
            }
        }

        if(!has_children){
            INTER_ON();
            return -1;
        }

        sleep(current_process);
    }
}

int sys_exit(void){
    uint8_t segment;
    uint16_t i;
    uint8_t old_frame;
    PCB* pcb;
    Proc* my_parent;

    if(current_process == init_process){
        panic("init exited");
    }

    pcb = MAP_PCB(current_process, old_frame);
    
    // TODO: close open files and cwd here using pcb->open_files

    for(segment = 0; segment < PAGE_TABLE_SIZE; segment++){
        if(pcb->page_table[segment] != FRAME_UNUSED){
            kfree(pcb->page_table[segment]);
            pcb->page_table[segment] = FRAME_UNUSED;
        }
    }
    
    pcb->exit_code = pcb->ctx.a;
    my_parent = pcb->parent; 
    
    UNMAP_PCB(old_frame); 

    INTER_OFF();
    wakeup(my_parent);
    current_process->state = PROC_STATE_ZOMBIE;

    // re-parent children to "init"
    for(i = 0; i < MAX_PROC_COUNT; i++){
        if(proc_table[i].state != PROC_STATE_UNUSED && proc_get_parent(&proc_table[i]) == current_process){
            proc_set_parent(&proc_table[i], init_process);
            if(proc_table[i].state == PROC_STATE_ZOMBIE){
                wakeup(init_process);
            }
        }
    }

    scheduler();
    panic("zombie exit");
    return -1;
}

#define FORK_CHUNK_SIZE 1024
int sys_fork(void){
    Proc* child;
    PCB* child_pcb;
    uint8_t segment;
    uint8_t parent_frame;
    uint8_t child_frame;
    uint8_t old_window1, old_window2;
    uint16_t offset;
    void *parent_buffer, *child_buffer;
    uint16_t child_pid;
    uint8_t parent_page_table[PAGE_TABLE_SIZE];

    child = palloc();
    if(!child) return -1;
    
    // NOTE: save the unique pid palloc() generated before we overwrite the PCB
    child_pid = proc_get_pid(child);
    
    // copy the kernel stack (this copies the entire PCB exactly)
    parent_buffer = mmu_map_window(1, current_process->kernel_low_memory[0], &old_window1);
    child_buffer  = mmu_map_window(2, child->kernel_low_memory[0], &old_window2);

    for(offset = 0; offset < 4096; offset += FORK_CHUNK_SIZE){
        memcpy((void*)((uint16_t)child_buffer + offset), (void*)((uint16_t)parent_buffer + offset), FORK_CHUNK_SIZE);
        
        INTER_OFF();
        current_process->state = PROC_STATE_READY;
        scheduler();
        INTER_ON();
    }
    
    // initialize the child PCB
    child_pcb = (PCB*)((uint16_t)child_buffer + PCB_OFFSET);
    child_pcb->pid = child_pid;            // restore correct pid
    child_pcb->parent = current_process;   // set parent
    child_pcb->ctx.a = 0;                  // child returns 0 from fork
    
    // save a copy of the page table to clone user memory, then wipe child's table
    memcpy(parent_page_table, child_pcb->page_table, PAGE_TABLE_SIZE);
    for(segment = 0; segment < PAGE_TABLE_SIZE; segment++){
        child_pcb->page_table[segment] = FRAME_UNUSED; 
    }

    mmu_unmap_window(1, old_window1);
    mmu_unmap_window(2, old_window2);

    // clone user memory pages
    for(segment = 0; segment < PAGE_TABLE_SIZE; segment++){
        parent_frame = parent_page_table[segment];
        
        if(parent_frame != FRAME_UNUSED){
            child_frame = kalloc();

            if(child_frame == FRAME_UNUSED){
                pfree(child); 
                return -1;
            }

            // map the child PCB briefly to update its page table with the new frame
            child_pcb = (PCB*)((uint16_t)mmu_map_window(2, child->kernel_low_memory[0], &old_window2) + PCB_OFFSET);
            child_pcb->page_table[segment] = child_frame;
            mmu_unmap_window(2, old_window2);

            // map user pages and copy
            parent_buffer = mmu_map_window(1, parent_frame, &old_window1);
            child_buffer  = mmu_map_window(2, child_frame, &old_window2);

            for(offset = 0; offset < 4096; offset += FORK_CHUNK_SIZE){
                memcpy((void*)((uint16_t)child_buffer + offset), (void*)((uint16_t)parent_buffer + offset), FORK_CHUNK_SIZE);
                INTER_OFF();
                current_process->state = PROC_STATE_READY;
                scheduler();
                INTER_ON();
            }

            mmu_unmap_window(1, old_window1);
            mmu_unmap_window(2, old_window2);
        }
    }

    child->state = PROC_STATE_NEW;
    return child_pid;
}

void run_init_process(void){

    PCB* init_pcb;
    uint8_t old_frame;

    const char* name = "init";

    init_process = palloc();
    if(!init_process){
        panic("palloc");
    }

    init_pcb = MAP_PCB(init_process, old_frame);

    // give the init process 1 frame to use
    init_pcb->page_table[0] = kalloc();
    if(init_pcb->page_table[0] == FRAME_UNUSED){
        panic("kalloc");
    }

    // manually open the 3 first file descriptors to the console
    init_pcb->open_files[0] = file_get_by_global_index(0); // stdin
    init_pcb->open_files[1] = file_get_by_global_index(0); // stdout
    init_pcb->open_files[2] = file_get_by_global_index(0); // stderr

    // inject the code to bootstrap the "/init" process
    init_pcb->ctx.pc = (uint16_t)init_code;
    init_pcb->top = (uint16_t)init_code + (uint16_t)_INITCODE_SIZE__;

    // name
    memcpy(init_pcb->name, name, strlen(name));

    UNMAP_PCB(old_frame);

    if(copy_to_user((void*)_INITCODE_LOAD__, (uint16_t)init_code, (uint16_t)_INITCODE_SIZE__, init_process) < 0){
        panic("copy_to_user");
    }

    init_process->state = PROC_STATE_NEW;
}
