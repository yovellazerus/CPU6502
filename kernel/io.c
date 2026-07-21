
#include "comman.h"

static char digits[] = "0123456789abcdef";
  
static void putc(char c){
    while(!(MMIO8(UART_STAT) & UART_STATUS_TX_READY)) {/* busy wait */};
    MMIO8(UART_TX) = c;
}

static void printint(long xx, int base, int sgn){
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

static void printptr(uint16_t x){
    int i;
    putc('0');
    putc('x');
    for (i = 0; i < (sizeof(uint16_t) * 2); i++, x <<= 4)
        putc(digits[x >> (sizeof(uint16_t) * 8 - 4)]);
}

// Only understands %d, %x, %p, %c, %s.
void vprintf(const char *fmt, va_list ap){
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

static char getc(void)
{
    while(!(MMIO8(UART_STAT) & UART_STATUS_RX_READY)) {/* busy wait */};
    return MMIO8(UART_RX);
}

uint16_t gets(char* buffer, int max)
{
    uint16_t i;
    char c;

    for (i = 0; i + 1 < max;) {
        c = getc();
        putc(c); // TODO: remove echo
        buffer[i++] = c;
        if (c == '\n' || c == '\r' || c == '\0')
            break;
    }
    buffer[i] = '\0';
    return i;
}