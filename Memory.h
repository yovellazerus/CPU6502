#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define MEMORY_SIZE (65536)
#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof(ARR[0]))
#define HIGH_BYTE(WORD) (WORD / 0x0100)
#define LOW_BYTE(WORD) (WORD % 0x0100)
#define IS_SIGN_BYTE(ARG) ((ARG & 0x80) != 0)
#define IS_6BIT_ON_BYTE(ARG) ((ARG & 0x40 != 0))
#define UNDEFINED_BYTE() 0

#define LOAD_LABEL(MEMORY, LABEL) (MEMORY->label_table[LABEL] = #LABEL)
#define NAME(ARG) (#ARG)

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

typedef unsigned char byte;
typedef unsigned short word;

typedef struct memory{
    byte data[MEMORY_SIZE];
    const char* label_table[MEMORY_SIZE]; // for debug  
} Memory;

void Memory_load_code(Memory* memory, const char* label, word entry_point, byte* code, size_t size);

void Memory_init(Memory* memory);
void Memory_dump_all(Memory* memory, FILE* stream);
void Memory_dump_stack(Memory* memory, byte sp, FILE* stream);





#endif // MEMORY_H