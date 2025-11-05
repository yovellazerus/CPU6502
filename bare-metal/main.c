// main.c — simple bare-metal C program for 6502

#define MMIO_SCR_DATA 0xC00A
#define MMIO_SCR_CTRL 0xC00B

#define SCR_WRITE 1

void main(void) {
    volatile unsigned char *SCR_DATA = (unsigned char *)MMIO_SCR_DATA;
    volatile unsigned char *SCR_CTRL = (unsigned char *)MMIO_SCR_CTRL;

    *SCR_DATA = 'H';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'I';
    *SCR_CTRL = SCR_WRITE;

    for (;;) {
        /* infinite loop */
    }
}
