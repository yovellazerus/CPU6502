#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

#define MEMORY_SIZE (64*KB)
#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define ZERO_PAGE_START 0x0000
#define ZERO_PAGE_END 0x00ff
#define PAGE_SIZE 0x0100

#define RESET_VECTOR_HIGH_BYTE 0xfffd
#define RESET_VECTOR_LOW_BYTE 0xfffc
// #define GLOBAL_PROGRAM_ENTRY_HIGH_BYTE 0x40
// #define GLOBAL_PROGRAM_ENTRY_LOW_BYTE 0x00

#define INTERRUPT_VECTOR_HIGH_BYTE 0xffff
#define INTERRUPT_VECTOR_LOW_BYTE 0xfffe
#define INTERRUPT_HANDLER_HIGH_BYTE 0x60
#define INTERRUPT_HANDLER_LOW_BYTE 0x00

#define NMI_VECTOR_HIGH_BYTE 0xfffb
#define NMI_VECTOR_LOW_BYTE 0xfffa
#define NMI_HANDLER_HIGH_BYTE 0x80
#define NMI_HANDLER_LOW_BYTE 0x00

#define SP_INIT_VALUE 0xfd
#define SP_HIGH_VALUE 0xff
#define SP_LOW_VALUE 0x00
#define STACK_HIGH_ADDRES 0x0100

typedef unsigned char byte;
typedef unsigned short word;

typedef struct memory{
    byte data[MEMORY_SIZE];
    const char* label_table[MEMORY_SIZE]; // for debug  
} Memory;

typedef struct program{
    const char* label;
    word entry_point;
    byte* code;
    size_t size;
} Program;

void Memory_program_init(Program* program, const char* label, word entry_point, byte* code, size_t size);
void Memory_load_program(Memory* memory, Program* program);
void Memory_load_code(Memory* memory, const char* label, word entry_point, byte* code, size_t size);

void Memory_init(Memory* memory, word entry_point);
void Memory_set_entry_point(Memory* memory, word entry_point);
void Memory_dump_all(Memory* memory, FILE* stream);
void Memory_dump_stack(Memory* memory, byte sp, FILE* stream);





#endif // MEMORY_H