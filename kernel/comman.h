#ifndef COMMAN_H
#define COMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../machine/machine.h"

#define NULL ((void*)0)

// 6502 "P" register flags
#define FLAG_N 0x80 // negative
#define FLAG_V 0x40 // overflow
#define FLAG_B 0x10 // break
#define FLAG_D 0x08 // decimal
#define FLAG_I 0x04 // interrupt Disable
#define FLAG_Z 0x02 // zero
#define FLAG_C 0x01 // carry

#define WINDOW1 0x1000
#define WINDOW2 0x2000
#define PAGE_TABLE_SIZE MMU_PAGE_TABLE_SIZE
#define FRAME_UNUSED MMU_FRAME_INVALID

#define CYCLES 100 // CPU steps per Timer tick

#define QUANTUM 10 // timer ticks until context switch

#define MAX_GLOBAL_OPEN_FILES 128
#define MAX_REGISTER_DEVICES  128

#define MAX_PROC_COUNT 512
#define MAX_PROC_NAME  16
#define MAX_FILES_PER_PROC 8

#define CACHE_SIZE 16
#define BLOCK_SIZE 4096

#define SIGKILL      (-1)
#define SEGFAULT     (-2)
#define BADSYSCALL   (-3)
#define WATCHDOG     (-4)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define BRK __asm__("brk"); __asm__("nop")
#define INTER_ON()  __asm__("cli")
#define INTER_OFF() __asm__("sei")

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define LOG(...) printk("\t[" __FILE__ ":" TOSTRING(__LINE__) "] " __VA_ARGS__); uart_putc_sync('\n');

#define MMIO8(register)  *(volatile uint8_t*)(register)
#define MMIO16(register) *(volatile uint16_t*)(register)
#define MMIO32(register) *(volatile uint32_t*)(register)

typedef uint8_t frame_t;
typedef struct Proc Proc;
typedef struct PCB PCB;

// order is importent for trampoline!
typedef struct Context {
    uint8_t sp;
    uint8_t p;
    uint16_t pc; 
    uint8_t x;
    uint8_t y;
    uint8_t a;
    uint8_t ksp;
} Context;

// form cc65
extern void* memset(void *dst, int value, uint16_t size);

// kernel.cfg
extern uint8_t _INITCODE_LOAD__[];
extern uint8_t _INITCODE_RUN__[];
extern uint8_t _INITCODE_SIZE__[];

// trampoline.s
extern uint8_t user_context[];
extern frame_t user_page_table[PAGE_TABLE_SIZE];
extern frame_t kernel_page_table[PAGE_TABLE_SIZE];
extern void return_from_trap(void);
extern void irq_handler(void);
extern void nmi_handler(void);
extern void kernel_vector(void);
extern void context_switch(Proc* old, Proc* new);
extern void first_context_switch(Proc* old, Proc* new);
extern void make_kernel_stack(frame_t frame);
extern void get_cpu_state(Context* ctx);

// initcode.s
extern uint8_t init_code[];

// kalloc.c
void kalloc_init(void);
frame_t kalloc(void);
void kfree(frame_t frame);

// syscall.c
typedef union SyscallArg{

    struct {
        uint32_t ticks;
    } sleep;

    struct {
        int fd;
        void* buffer;
        uint16_t size;
    } read;

    struct {
        int fd;
        void* buffer;
        uint16_t size;
    } write;

    struct {
        const char* path;
        uint16_t mode;
        uint16_t flags;
    } open;
    
    // ...

} SyscallArg;

typedef int (*Syscall)(void);
extern Syscall syscalls_table[256];
void syscall_init(void);
bool syscall_populate_argument(SyscallArg* arg);

#define SYS_FORK    'F'
#define SYS_EXIT    'E'
#define SYS_WAIT    'W'
#define SYS_KILL    'K'
#define SYS_SBRK    'B'
#define SYS_SLEEP   'S'
#define SYS_GETPID  'G'

#define SYS_OPEN     'o'
#define SYS_CLOSE    'c'
#define SYS_READ     'r'
#define SYS_WRITE    'w'

// trap.c
void kernel_brk(void);
void kernel_irq(void);
void kernel_nmi(void);
uint8_t device_interrupt(void);
void interrupts_push(void);
void interrupts_pop(void);
void kernel_prologue(void);
void kernel_epilogue(void);

// debugger.c
void kernel_debugger(void);
void print_cpu_state(Context* ctx);
void print_process_state(Proc* p);
uint16_t gets(char *buf, int max);

// buffer.c

#define BUFFER_FLAGS_BUSY  0x01
#define BUFFER_FLAGS_VALID 0x02
#define BUFFER_FLAGS_DIRTY 0x04

typedef struct Block_Buffer {
    uint8_t flags;
    uint8_t drive;
    uint8_t refcount;
    uint16_t block_number;
    struct Block_Buffer* next;
    struct Block_Buffer* prev;
    struct Block_Buffer* queue;
    frame_t frame;
} Block_Buffer;

void buffer_init(void);
Block_Buffer* buffer_read(uint8_t drive, uint16_t block_number);
void buffer_write(Block_Buffer* b);
void buffer_release(Block_Buffer* b);
void buffer_pin(Block_Buffer* b);
void buffer_unpin(Block_Buffer* b);

// disk.c
void disk_init(void);
void disk_interrupt(void);
void disk_block_read(Block_Buffer* b);
void disk_block_write(Block_Buffer* b);

// mmu.c
void  mmu_init(void);
void* mmu_map_window(uint8_t window, frame_t frame, frame_t* out_old_frame);
void  mmu_unmap_window(uint8_t window, frame_t old_frame);

// uart.c
typedef struct Ring_Buffer Ring_Buffer;
extern Ring_Buffer ring_buffer;
void uart_init(void);
void uart_rx_interrupt(void);
void uart_putc_sync(char c);
void uart_putc(char c);
int  uart_getc_sync(void);
int  uart_getc(void);

// vfs.c
typedef enum {
    VFILE_TYPE_DEVICE = 0,
    VFILE_TYPE_INODE
} VFile_Type;

typedef enum {
    DEVICE_MAJOR_CONSOLE = 0,
    DEVICE_MAJOR_DISK,
} Device_Major;

typedef struct File File;

typedef struct Device_Ops {
    int (*read)(File*, void*, uint16_t);
    int (*write)(File*, void*, uint16_t);
    int (*close)(File*);
} Device_Ops;

void  vfs_init(void);
File* file_get_by_global_index(uint8_t index);
bool  file_open_global(uint8_t index,
                        VFile_Type type,
                        Device_Major major,
                        uint8_t  readable,
                        uint8_t  writable,
                        uint32_t offset);
bool  register_device(Device_Major major, Device_Ops* devops);

int sys_read(void);
int sys_write(void);
int sys_close(void);
int sys_open(void);

// consol.c
void console_init(void);
int  console_read(File* file, void* dst, uint16_t n);
int  console_write(File* file, void* src, uint16_t n);
int  console_close(File* file);

// printk.c
void panic(const char *fmt, ...);
void printk(const char *fmt, ...);
void vprintk(const char *fmt, va_list ap);

// timer.c
extern volatile uint32_t systicks;
extern volatile uint32_t next_wakeup_call;
void timer_init(void);
void timer_pause(void);
void timer_resume(void);
void timer_interrupt(void);

// string.c
char*    strcpy(char *s, const char *t);
int      strcmp(const char *p, const char *q);
uint16_t strlen(const char *s);
char*    strchr(const char* s, char c);
int      atoi(const char *s);
void*    memmove(void *vdst, const void *vsrc, int n);
int      memcmp(const void *s1, const void *s2, uint16_t n);
void*    memcpy(void *dst, const void *src, uint16_t n);

// proc.c
typedef enum Proc_State{
    PROC_STATE_UNUSED = 0,
    PROC_STATE_BUILDING,
    PROC_STATE_NEW,
    PROC_STATE_READY,
    PROC_STATE_RUNING,
    PROC_STATE_SLEEPING,
    PROC_STATE_ZOMBIE
} Proc_State;

extern Proc* current_process;

int sys_fork(void);
int sys_exit(void);
int sys_wait(void);
int sys_kill(void);
int sys_sbrk(void);
int sys_sleep(void);
int sys_getpid(void);

void sleep(void* channel);
void wakeup(void* channel);
void scheduler(void);
void run_init_process(void);
int8_t  copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, Proc* p);
int8_t  copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, Proc* p);
void  proc_init(void);
Proc* palloc(void);
void  pfree(Proc* p);

// CPU Context
uint16_t proc_get_ax(const Proc* p);
uint8_t  proc_get_y(const Proc* p);
void     proc_set_ax(Proc* p, uint16_t ax);
void     proc_set_a(Proc* p, uint8_t a);
void     proc_get_ctx(const Proc* p, Context* ctx);
void     proc_set_ctx(Proc* p, Context* ctx);

// memory management
frame_t* proc_get_kernel_low_memory(Proc* p);
void     proc_get_page_table(const Proc* p, frame_t page_table[PAGE_TABLE_SIZE]);
void     proc_set_page_table(Proc* p, frame_t page_table[PAGE_TABLE_SIZE]);
uint16_t proc_get_top(const Proc* p);
void     proc_set_top(Proc* p, uint16_t top);

// process Hierarchy and Identity
uint16_t proc_get_pid(const Proc* p);
uint8_t  proc_get_uid(const Proc* p);
void     proc_get_name(const Proc* p, char name[MAX_PROC_NAME]);
Proc*    proc_get_parent(const Proc* p);
void     proc_set_parent(Proc* p, Proc* parent);

// scheduling and Lifecycle
void     proc_set_state(Proc* p, Proc_State state);
uint8_t  proc_get_ticks(const Proc* p);
void     proc_set_ticks(Proc* p, uint8_t ticks);
uint8_t  proc_ticks_dec(Proc* p);
uint16_t proc_get_killed(const Proc* p);
void     proc_set_killed(Proc* p, uint16_t killed);
uint8_t  proc_get_exit_code(const Proc* p);

// file System
File*    proc_get_file(const Proc* p, int fd);

#endif // COMMAN_H