#ifndef COMMAN_H
#define COMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../machine/machine.h"

#define MAX_FILES_PER_PROC 8

#define WINDOW1 0x1000
#define PAGE_TABLE_SIZE 16
#define FRAME_UNUSED 0

#define MAX_PROC_COUNT 64
#define QUANTUM 10

#define TODO() panic("function \"%s\" is not implemented", __func__)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define MMIO8(register)  *(volatile uint8_t*)(register)
#define MMIO16(register) *(volatile uint16_t*)(register)
#define MMIO32(register) *(volatile uint32_t*)(register)

typedef enum Proc_State{
    PROC_STATE_UNUSED = 0,
    PROC_STATE_USED,
    PROC_STATE_READY,
    PROC_STATE_RUNING,
    PROC_STATE_WAITING,
    PROC_STATE_ZOMBIE
} Proc_State;

typedef struct Context {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint8_t p;
    uint16_t pc; 
} Context;

typedef struct Proc {

    Context ctx; 
    uint8_t page_table[PAGE_TABLE_SIZE];
    uint16_t sz;
     
    Proc_State state; 
    uint16_t pid;                   
    uint8_t  uid;           
    uint8_t  gid;           
    uint8_t  ecode;         
    uint8_t  priority;      
    uint8_t  ticks;   

    struct Proc* parent;    
    void* wchan;       
    bool  killed;      

    uint16_t cwd_inode;     
    uint8_t  fd_table[MAX_FILES_PER_PROC];          

    // debug 
    char name[16];

} Proc;

// trampoline.s
extern void return_from_trap(uint8_t* page_table);
extern void irq_handler(void);
extern void nmi_handler(void);

// kalloc.c
void kalloc_init(void);
uint8_t kalloc(void);
void kfree(uint8_t frame);

// trap.c
void _syscall_entry(void);
void _kernel_irq(void);
void _kernel_nmi(void);

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
extern void* memset(void *dst, int value, uint16_t size);
void*   memcpy(void *dst, const void *src, uint16_t n);

// proc.c
void scheduler(void);
int8_t copy_from_user(void* kernel_dest, uint16_t user_src, uint16_t n, uint8_t* page_table);

#endif // COMMAN_H

