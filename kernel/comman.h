#ifndef COMMAN_H
#define COMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../machine/machine.h"

#define NULL ((void*)0)

#define MAX_FILES_PER_PROC 8

#define WINDOW1 0x1000
#define WINDOW2 0x2000
#define PAGE_TABLE_SIZE 16
#define FRAME_UNUSED 0

#define MAX_PROC_COUNT 64
#define QUANTUM 10

#define TODO() panic("function \"%s\" is not implemented", __func__)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define MMIO8(register)  *(volatile uint8_t*)(register)
#define MMIO16(register) *(volatile uint16_t*)(register)
#define MMIO32(register) *(volatile uint32_t*)(register)

// form cc65
extern void* memset(void *dst, int value, uint16_t size);

// trampoline.s
extern uint8_t life_raft[];
extern uint8_t kernel_page_table[PAGE_TABLE_SIZE];
extern void return_from_trap(void);
extern void irq_handler(void);
extern void nmi_handler(void);

// kalloc.c
void kalloc_init(void);
uint8_t kalloc(void);
void kfree(uint8_t frame);

// syscall.c
typedef union Syscall_Arg Syscall_Arg;
typedef int (*Syscall)(void);
extern Syscall syscalls_table[256];
void syscall_init(void);

#define SYS_WRITE 'Y'

int sys_write(void);

// trap.c
void kernel_brk(void);
void kernel_irq(void);
void kernel_nmi(void);

// io.c
void panic(const char *fmt, ...);
void printk(const char *fmt, ...);
void vprintk(const char *fmt, va_list ap);
uint16_t gets(char *buf, int max);

// timer.c
void timer_init(void);
void timer_puse(void);
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
    PROC_STATE_USED,
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
} Context;

typedef struct Proc Proc;
extern Proc* current_process;

void interrupts_enable(void);
void interrupts_disable(void);
void sleep(void* channel);
void wakeup(void* channel);
void scheduler(void);
void run_init_process(void);
void copy_to_life_raft(Proc* p);
void copy_from_life_raft(Proc* p);
int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, const uint8_t* page_table);
int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, uint8_t* page_table);
void proc_init(void);
Proc* palloc(void);
const Context* proc_get_ctx(const Proc* p);
void proc_set_ax(Proc* p, uint16_t ax);
const uint8_t* proc_get_page_table(const Proc* p);

#endif // COMMAN_H

