#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define HIGH_BYTE(WORD) (WORD / 0x0100)
#define LOW_BYTE(WORD) (WORD % 0x0100)
#define IS_CROSS_PAGES(old, _new) ((old & 0xFF00) != (_new & 0xFF00))

#define IS_NEGATIVE(arg) ((arg & 0x80) != 0)
#define IS_OVERFLOW(reg_old, reg_new, operand) (((reg_old ^ reg_new) & (operand ^ reg_new) & 0x80) != 0)

#define MEMORY_SIZE (256*256)

#define ZERO_PAGE_START 0x0000
#define ZERO_PAGE_END 0x00ff
#define STACK_START 0x0100
#define STACK_END 0x01ff
#define PAGE_SIZE 0x0100

#define RESET_VECTOR_HIGH_ADDER 0xfffd
#define RESET_VECTOR_LOW_ADDER 0xfffc
#define ENTRY_POINT_ADDERS 0x8000 /*program start at 0x8000 for now,There is no operating system yet, 
                                            so the program starts here without any special preparations.*/
#define IRQ_VECTOR_HIGH_ADDER 0xffff
#define IRQ_VECTOR_LOW_ADDER 0xfffe
#define IRQ_HANDLER_ADDRES 0xff80

#define NMI_VECTOR_HIGH_ADDER 0xfffb
#define NMI_VECTOR_LOW_ADDER 0xfffa
#define NMI_HANDLER_ADDRES 0xff00

#define UNDEFINED_BYTE() (0)

typedef uint8_t byte;
typedef uint16_t word;

typedef enum {
    COLOR_RESET,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
} Color;

void set_color(Color color, FILE* file);

#endif // UTILITIES_H_
