#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define UNUSED ((void)cpu)

#define HIGH_BYTE(WORD) ((byte)(WORD >> 8))
#define LOW_BYTE(WORD) ((byte)(WORD << 8))
#define IS_CROSS_PAGES(old, _new) ((old & 0xFF00) != (_new & 0xFF00))

#define IS_ZERO(reg) (reg == 0)
#define IS_NEGATIVE(reg) ((reg & 0x80) != 0)
#define IS_OVERFLOW(reg, operand) (((reg ^ (reg + operand)) & ((operand ^ (reg + operand)) & 0x80)) != 0)
#define IS_CARRY(reg, operand) (HIGH_BYTE(reg + operand) > 0)

#define MEMORY_SIZE (256*256)
#define ZERO_PAGE_START 0x0000
#define ZERO_PAGE_END 0x00ff
#define STACK_START 0x0100
#define STACK_END 0x01ff
#define PAGE_SIZE 0x0100

#define RESET_VECTOR_HIGH_ADDER 0xfffd
#define RESET_VECTOR_LOW_ADDER 0xfffc
#define RESET_HANDLER 0x8000 /*program start at 0x8000 for now,There is no operating system yet, 
                                            so the program starts here without any special preparations.*/
#define IRQ_VECTOR_HIGH_ADDER 0xffff
#define IRQ_VECTOR_LOW_ADDER 0xfffe
#define IRQ_HANDLER 0xff80

#define NMI_VECTOR_HIGH_ADDER 0xfffb
#define NMI_VECTOR_LOW_ADDER 0xfffa
#define NMI_HANDLER 0xff00

typedef uint8_t byte;
typedef uint16_t word;

typedef struct CPU_t {

    byte memory[MEMORY_SIZE];

    word PC;
    byte SP;
    byte P;

    byte A;
    byte X;
    byte Y;

} CPU;

typedef enum {
    Add_non = 0,
    Add_Brk,
    Add_Relative,
    Add_Implied,
    Add_Immediate,
    Add_Accumulator,

    Add_Absolute,
    Add_AbsoluteX,
    Add_AbsoluteY,

    Add_ZeroPage,
    Add_ZeroPageX,
    Add_ZeroPageY,

    Add_Indirect,
    Add_IndirectX,
    Add_IndirectY,

    
    count_Add,
} Addressing_mode;

extern Addressing_mode opcode_to_Addressing_mode[0xff + 1];

extern const char* opcode_to_cstr[0xff + 1];

typedef void (*Instruction)(CPU*);

extern Instruction Opcode_to_Instruction_table[0xff + 1];

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
