#ifndef COMMAN_H
#define COMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../machine/machine.h"

#define NULL ((void*)0)

#define P_I 0x04

#define WINDOW1 0x1000
#define WINDOW2 0x2000
#define PAGE_TABLE_SIZE MMU_PAGE_TABLE_SIZE
#define FRAME_UNUSED MMU_FRAME_INVALID

#define CYCLES 100 // CPU steps per Timer tick

#define QUANTUM 10 // timer ticks until context switch

#define MAX_PROC_COUNT 64
#define MAX_PROC_NAME  16
#define MAX_FILES_PER_PROC 8

#define SIGKILL      (-1)
#define SEGFAULT     (-2)
#define BADSYSCALL   (-3)
#define WATCHDOG     (-4)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define MMIO8(register)  *(volatile uint8_t*)(register)
#define MMIO16(register) *(volatile uint16_t*)(register)
#define MMIO32(register) *(volatile uint32_t*)(register)

typedef struct Proc Proc;

// form cc65
extern void* memset(void *dst, int value, uint16_t size);

// trampoline.s
extern uint8_t life_raft[];
extern uint8_t kernel_page_table[PAGE_TABLE_SIZE];
extern void return_from_trap(void);
extern void irq_handler(void);
extern void nmi_handler(void);
extern void kernel_vector(void);
extern void context_switch(Proc* old, Proc* new);
extern void first_context_switch(Proc* old, Proc* new);
extern void make_kernel_stack(uint8_t frame);

// initcode.s
extern uint8_t init_code[];
extern uint8_t _INITCODE_LOAD__[];
extern uint8_t _INITCODE_RUN__[];
extern uint8_t _INITCODE_SIZE__[];

// kalloc.c
void memory_init(void);
uint8_t kalloc(void);
void kfree(uint8_t frame);

// syscall.c
typedef union SyscallArg SyscallArg;
typedef int (*Syscall)(void);
void syscall_init(void);

extern Syscall syscalls_table[256];

#define SYS_PRINT   'P'
#define SYS_FORK    'F'
#define SYS_EXIT    'E'
#define SYS_WAIT    'W'
#define SYS_KILL    'K'

int sys_print(void);
int sys_fork(void);
int sys_exit(void);
int sys_wait(void);
int sys_kill(void);

// trap.c
void kernel_brk(void);
void kernel_irq(void);
void kernel_nmi(void);
void kernel_software_interrupt(void);
bool device_interrupt(void);

// io.c
// TODO: need to be uart.c console.c and printk.c
void panic(const char *fmt, ...);
void printk(const char *fmt, ...);
void vprintk(const char *fmt, va_list ap);
// debug
void print_str(char* str);
uint16_t gets(char *buf, int max);

// timer.c
extern volatile uint32_t systicks;
void timer_init(void);
void timer_pause(void);
void timer_resume(void);

// string.c
char*   strcpy(char *s, const char *t);
int     strcmp(const char *p, const char *q);
uint16_t strlen(const char *s);
char*   strchr(const char* s, char c);
int     atoi(const char *s);
void*   memmove(void *vdst, const void *vsrc, int n);
int     memcmp(const void *s1, const void *s2, uint16_t n);
void*   memcpy(void *dst, const void *src, uint16_t n);

// proc.c
typedef enum Proc_State{
    PROC_STATE_UNUSED = 0,
    PROC_STATE_NEW,
    PROC_STATE_READY,
    PROC_STATE_RUNING,
    PROC_STATE_SLEEPING,
    PROC_STATE_ZOMBIE
} Proc_State;

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

extern Proc* current_process;

void interrupts_push(void);
void interrupts_pop(void);
void sleep(void* channel);
void wakeup(void* channel);
void scheduler(void);
void run_init_process(void);
void kernel_prologue(void);
void kernel_epilogue(void);
int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, const uint8_t* page_table);
int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, uint8_t* page_table);
void proc_init(void);
Proc* palloc(void);
void pfree(Proc* p);
const Context* proc_get_ctx(const Proc* p);
uint8_t proc_get_ticks(const Proc* p);
const char* proc_get_name(const Proc* p);
uint16_t proc_get_pid(const Proc* p);
uint8_t proc_ticks_dec(Proc* p);
uint16_t proc_get_ax(const Proc* p);
void proc_set_ax(Proc* p, uint16_t ax);
void proc_set_state(Proc* p, Proc_State state);
const uint8_t* proc_get_page_table(const Proc* p);

#endif // COMMAN_H
