
#include <stdint.h>
#include <stdarg.h>

// ======= memory map ==============================================================================

#define RAM_BASE     0x0000
#define RAM_SIZE     0xfc00

#define DISK_BUF     0xfc00 // 512 bytes
#define DISK_STAT    0xfe00
#define DISK_CMD     0xfe01
#define DISK_LBA     0xfe02

#define UART_TX      0xfe10
#define UART_RX      0xfe11

#define MMU_MAP      0xfe20 // 16 bytes

#define ROM_BASE     0xff00
#define ROM_SIZE     0x0100

#define MMIO(register) *(volatile uint8_t*)(register)

// ======= utils ======================================================================================

static char digits[] = "0123456789ABCDEF";

static void putc(char c)
{
    while(MMIO(UART_TX)) {/* busy wait */};
    MMIO(UART_TX) = c;
}

static void printint(long xx, int base, int sgn)
{
  char buf[20];
  int i, neg;
  unsigned long x;

  neg = 0;
  if (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } else {
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
      } else {
        putc(c0);
      }
    } else if (state == '%') {
      c1 = c2 = 0;
      if (c0)
        c1 = fmt[i + 1] & 0xff;
      if (c1)
        c2 = fmt[i + 2] & 0xff;
      if (c0 == 'd') {
        printint(va_arg(ap, int), 10, 1);
      } else if (c0 == 'l' && c1 == 'd') {
        printint(va_arg(ap, long), 10, 1);
        i += 1;
      } else if (c0 == 'u') {
        printint(va_arg(ap, uint16_t), 10, 0);
      } else if (c0 == 'l' && c1 == 'u') {
        printint(va_arg(ap, uint32_t), 10, 0);
        i += 1;
      } else if (c0 == 'x') {
        printint(va_arg(ap, uint16_t), 16, 0);
      } else if (c0 == 'l' && c1 == 'x') {
        printint(va_arg(ap, uint32_t), 16, 0);
        i += 1;
      } else if (c0 == 'p') {
        printptr(va_arg(ap, uint16_t));
      } else if (c0 == 'c') {
        putc(va_arg(ap, uint16_t));
      } else if (c0 == 's') {
        if ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        for (; *s; s++)
          putc(*s);
      } else if (c0 == '%') {
        putc('%');
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc('%');
        putc(c0);
      }

      state = 0;
    }
  }
}

void
fprintf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

// ======= main =====================================================================================

void main(void) {

    int x = -42;
    
    printf("number: %d, hex: %x, unsigned: %u, char: %c, string: \"%s\", pointer: %p %%\n",
        x, 0xffcb, 0xffff, 'G', "hello world!", &x  
    );

    while (1);  // loop forever
}
