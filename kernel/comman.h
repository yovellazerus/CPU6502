#ifndef COMMAN_H
#define COMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../machine/machine.h"

#define TODO() panic("function \"%s\" is not implemented", __func__)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define MMIO8(register)  *(volatile uint8_t*)(register)
#define MMIO16(register) *(volatile uint16_t*)(register)
#define MMIO32(register) *(volatile uint32_t*)(register)

// io.c
void panic(const char *fmt, ...);
void printf(const char *fmt, ...);
void vprintf(const char *fmt, va_list ap);
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

#endif // COMMAN_H