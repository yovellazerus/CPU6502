
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../machine/machine.h"

#define MMIO8(register)  *(volatile uint8_t*)(register)
#define MMIO16(register) *(volatile uint16_t*)(register)
#define MMIO32(register) *(volatile uint32_t*)(register)

// ======= utils ======================================================================================

void panic(const char *fmt, ...);

char * strcpy(char *s, const char *t)
{
    char *os;

    os = s;
    while ((*s++ = *t++) != 0)
        ;
    return os;
}

int
strcmp(const char *p, const char *q)
{
    while (*p && *p == *q)
        p++, q++;
    return (uint8_t)*p - (uint8_t)*q;
}

uint16_t strlen(const char *s)
{
    int n;

    for (n = 0; s[n] && n < (uint16_t)-1; n++)
        ;
    return n;
}

char* strchr(const char* s, char c)
{
    for (; *s; s++)
        if (*s == c)
        return (char *)s;
    return 0;
}

int atoi(const char *s)
{
    int n;
    
    n = 0;
    while ('0' <= *s && *s <= '9')
      n = n * 10 + *s++ - '0';
    return n;
}

void* memmove(void *vdst, const void *vsrc, int n)
{
    char *dst;
    const char *src;

    dst = vdst;
    src = vsrc;
    if (src > dst) {
        while (n-- > 0)
        *dst++ = *src++;
    } else {
        dst += n;
        src += n;
        while (n-- > 0)
        *--dst = *--src;
    }
    return vdst;
}

int memcmp(const void *s1, const void *s2, uint16_t n)
{
    const char *p1 = s1, *p2 = s2;
    while (n-- > 0) {
        if (*p1 != *p2) {
        return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

extern void* memset(void *dst, int value, uint16_t size);

void* memcpy(void *dst, const void *src, uint16_t n)
{
    return memmove(dst, src, n);
}

// static void disk_read_sector(uint8_t* buffer, uint16_t lba){
//     MMIO16(DISK_LBA) = lba;
//     MMIO8(DISK_CMD) = DISK_CMD_READ;
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     memcpy(buffer, (uint8_t*)DISK_BUF, DISK_SECTOR_SIZE);
// }

// static void disk_write_sector(uint8_t* buffer, uint16_t lba){
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     MMIO16(DISK_LBA) = lba;
//     MMIO8(DISK_CMD) = DISK_CMD_WRITE;
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     memcpy((uint8_t*)DISK_BUF, buffer , DISK_SECTOR_SIZE);
// }

static char digits[] = "0123456789ABCDEF";
  
static void putc(char c)
{
    while(!(MMIO8(UART_STAT) & UART_STATUS_TX_READY)) {/* busy wait */};
    MMIO8(UART_TX) = c;
}

// static char getc(void)
// {
//     while(!(MMIO8(UART_STAT) & UART_RX_READY)) {/* busy wait */};
//     return MMIO8(UART_RX);
// }

// uint16_t gets(char *buf, int max)
// {
//     uint16_t i;
//     char c;

//     for (i = 0; i + 1 < max;) {
//         c = getc();
//         putc(c);
//         buf[i++] = c;
//         if (c == '\n' || c == '\r' || c == '\0')
//             break;
//     }
//     buf[i] = '\0';
//     return i;
// }

static void printint(long xx, int base, int sgn)
{
    char buf[20];
    int i, neg;
    unsigned long x;

    neg = 0;
    if (sgn && xx < 0) {
        neg = 1;
        x = -xx;
    } 
    else {
        x = xx;
    }

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0)
        putc(buf[i]);
}

static void printptr(uint16_t x)
{
    int i;
    putc('0');
    putc('x');
    for (i = 0; i < (sizeof(uint16_t) * 2); i++, x <<= 4)
        putc(digits[x >> (sizeof(uint16_t) * 8 - 4)]);
}

// Only understands %d, %x, %p, %c, %s.
void vprintf(const char *fmt, va_list ap)
{
    char *s;
    int c0, c1, c2, i, state;

    state = 0;
    for (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    if (state == 0) {
        if (c0 == '%') {
            state = '%';
        } 
        else {
            putc(c0);
        }
        } 
        else if (state == '%') {
        c1 = c2 = 0;
        if (c0)
         c1 = fmt[i + 1] & 0xff;
        if (c1)
            c2 = fmt[i + 2] & 0xff;
        if (c0 == 'd') {
            printint(va_arg(ap, int), 10, 1);
        } 
        else if (c0 == 'l' && c1 == 'd') {
            printint(va_arg(ap, long), 10, 1);
            i += 1;
        } 
        else if (c0 == 'u') {
            printint(va_arg(ap, uint16_t), 10, 0);
        } 
        else if (c0 == 'l' && c1 == 'u') {
            printint(va_arg(ap, uint32_t), 10, 0);
            i += 1;
        } 
        else if (c0 == 'x') {
            printint(va_arg(ap, uint16_t), 16, 0);
        } 
        else if (c0 == 'l' && c1 == 'x') {
            printint(va_arg(ap, uint32_t), 16, 0);
            i += 1;
        } 
        else if (c0 == 'p') {
            printptr(va_arg(ap, uint16_t));
        } 
        else if (c0 == 'c') {
            putc(va_arg(ap, uint16_t));
        } 
        else if (c0 == 's') {
            if ((s = va_arg(ap, char *)) == 0)
            s = "(null)";
            for (; *s; s++)
            putc(*s);
        } 
        else if (c0 == '%') {
            putc('%');
        } 
        else {
            // Unknown % sequence.  Print it to draw attention.
            putc('%');
            putc(c0);
        }
        state = 0;
        }
    }
}

void printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

void panic(const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    printf("PANIC: ");
    vprintf(fmt, ap);
    va_end(ap);
}

// ======= main =====================================================================================

void main(void) {

    int x = -42;

    printf("****** 6502 kernel ******\n");
    
    printf("number: %d, hex: 0x%x, unsigned: %u, char: %c, string: \"%s\", pointer: %p %%\n",
                    x,      0xffcb,        0xffff,     'G',    "hello world!",      &x  
    );

    while (true) {/* halt */}
}
