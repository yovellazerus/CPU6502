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

#define MEM_SIZE (64*KB)

#define RESET_VECTOR_HIGH_BYTE 0xfffd
#define RESET_VECTOR_LOW_BYTE 0xfffc
#define PROGRAM_ENTRY_HIGH_BYTE 0x00
#define PROGRAM_ENTRY_LOW_BYTE 0x00

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
    byte data[MEM_SIZE];  
} Memory;

void Memory_init(Memory* mem);
void Memory_dump_all(Memory* mem, FILE* stream);
void Memory_dump_stack(Memory* mem, byte sp, FILE* stream);




#endif // MEMORY_H