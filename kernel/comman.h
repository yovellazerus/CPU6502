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

// trap.c
void kernel_brk(void);
void kernel_irq(void);
void kernel_nmi(void);

// io.c
void panic(const char *fmt, ...);
void printk(const char *fmt, ...);
void vprintk(const char *fmt, va_list ap);
uint16_t gets(char *buf, int max);

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
typedef enum Proc_State Proc_State;
typedef struct Context Context;
typedef struct Proc Proc;
void scheduler(void);
void run_init_process(void);
void copy_to_life_raft(const Context* ctx, uint8_t* user_page_table);
int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, uint8_t* page_table);
int8_t copy_to_user(void* kernel_src, uint16_t user_dest, uint16_t n, uint8_t* page_table);
void proc_init(void);
Proc* palloc(void);

#endif // COMMAN_H

