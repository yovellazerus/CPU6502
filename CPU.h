#ifndef CPU_H
#define CPU_H

#include "Memory.h"

#define IS_SIGN_BYTE(ARG) ((ARG & 0b10000000) != 0)

typedef struct cpu{
    byte A;
    byte X;
    byte Y;
    word PC;
    byte SP;
    byte Flags;
    
    size_t 	cycles;
    const char* name; 
} CPU;

void CPU_dump(CPU* cpu, FILE* stream);
bool CPU_offFlag(CPU* cpu, char flag);
bool CPU_onFlag(CPU* cpu, char flag);
bool CPU_getFlag(CPU* cpu, char flag);
void CPU_init(CPU* cpu, const char* name);

void CPU_execute(CPU* cpu, Memory* mem);
void CPU_tick(CPU* cpu, size_t amount);
void CPU_invalid_opcode(CPU* cpu, byte opcode);

// operations:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define NUMBER_OF_OPERATIONS 0xff

typedef void (*operation)(CPU*, Memory*);

// LDA:
void operation_LDA_Immediate(CPU* cpu, Memory* mem);
void operation_LDA_Absolute(CPU* cpu, Memory* mem);
void operation_LDA_Absolute_X(CPU* cpu, Memory* mem);
void operation_LDA_Absolute_Y(CPU* cpu, Memory* mem);
void operation_LDA_Zero_Page(CPU* cpu, Memory* mem);
void operation_LDA_Zero_Page_X(CPU* cpu, Memory* mem);
void operation_LDA_indirect_X(CPU* cpu, Memory* mem);
void operation_LDA_indirect_Y(CPU* cpu, Memory* mem);

// LDX:

// LDY:

// STA:
void operation_STA_Absolute(CPU* cpu, Memory* mem);

// STX:

// STY:



static operation operation_table[NUMBER_OF_OPERATIONS] = {
    // LDA:
    [0xa9] = operation_LDA_Immediate,
    [0xad] = operation_LDA_Absolute,
    [0xbd] = operation_LDA_Absolute_X,
    [0xb9] = operation_LDA_Absolute_Y,
    [0xa5] = operation_LDA_Zero_Page,
    [0xb5] = operation_LDA_Zero_Page_X,
    [0xa1] = operation_LDA_indirect_X,
    [0xb1] = operation_LDA_indirect_Y,

    // LDX:

    // LDY:

    // STA:
    [0x8d] = operation_STA_Absolute,

    // STX:

    // STY:
    
};

#endif // CPU_H