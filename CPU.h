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
    byte P;
    
    size_t 	cycles;
    const char* name; 
    bool hlt;
} CPU;

void CPU_dump(CPU* cpu, FILE* stream);
bool CPU_offFlag(CPU* cpu, char flag);
bool CPU_onFlag(CPU* cpu, char flag);
bool CPU_getFlag(CPU* cpu, char flag);
void CPU_init(CPU* cpu, const char* name);

void CPU_execute(CPU* cpu, Memory* mem);
void CPU_reset(CPU* cpu, Memory* mem);
void CPU_tick(CPU* cpu, size_t amount);
void CPU_invalid_opcode(CPU* cpu, byte opcode);

// operations:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define NUMBER_OF_POSSIBLE_OPERATIONS (0xff + 1)

typedef void (*operation)(CPU*, Memory*);

void operation_LDA_Immediate(CPU* cpu, Memory* mem);
void operation_LDA_Absolute(CPU* cpu, Memory* mem);
void operation_LDA_Absolute_X(CPU* cpu, Memory* mem);
void operation_LDA_Absolute_Y(CPU* cpu, Memory* mem);
void operation_LDA_Zero_Page(CPU* cpu, Memory* mem);
void operation_LDA_Zero_Page_X(CPU* cpu, Memory* mem);
void operation_LDA_indirect_X(CPU* cpu, Memory* mem);
void operation_LDA_indirect_Y(CPU* cpu, Memory* mem);

void operation_STA_Absolute(CPU* cpu, Memory* mem);

void operation_TAX_Implied(CPU* cpu, Memory* mem);
void operation_TAY_Implied(CPU* cpu, Memory* mem);
void operation_TXA_Implied(CPU* cpu, Memory* mem);
void operation_TYA_Implied(CPU* cpu, Memory* mem);

void operation_PHA_Implied(CPU* cpu, Memory* mem);
void operation_PLA_Implied(CPU* cpu, Memory* mem);

void operation_JMP_Absolute(CPU* cpu, Memory* mem);
void operation_JMP_Indirect(CPU* cpu, Memory* mem);
void operation_JSR_Absolute(CPU* cpu, Memory* mem);
void operation_RTS_Implied(CPU* cpu, Memory* mem);

void operation_BRK_Implied(CPU* cpu, Memory* mem);
void operation_NOP_Implied(CPU* cpu, Memory* mem);
void operation_RTI_Implied(CPU* cpu, Memory* mem);

static operation operation_table[NUMBER_OF_POSSIBLE_OPERATIONS] = {
    
    [0xa9] = operation_LDA_Immediate,
    [0xad] = operation_LDA_Absolute,
    // [0xbd] = operation_LDA_Absolute_X,
    // [0xb9] = operation_LDA_Absolute_Y,
    // [0xa5] = operation_LDA_Zero_Page,
    // [0xb5] = operation_LDA_Zero_Page_X,
    // [0xa1] = operation_LDA_indirect_X,
    // [0xb1] = operation_LDA_indirect_Y,

    [0x8d] = operation_STA_Absolute,

    [0xaa] = operation_TAX_Implied,
    [0xa8] = operation_TAY_Implied,
    [0x8a] = operation_TXA_Implied,
    [0x98] = operation_TYA_Implied,

    [0x48] = operation_PHA_Implied,
    [0x68] = operation_PLA_Implied,

    [0x4c] = operation_JMP_Absolute,
    [0x6c] = operation_JMP_Indirect,

    [0x20] = operation_JSR_Absolute,

    [0x60] = operation_RTS_Implied,

    [0xff] = operation_BRK_Implied, // TODO: in reality it is 0x00...modified for dubbing!  
    [0xea] = operation_NOP_Implied,
    [0x40] = operation_RTI_Implied,
    
};

#endif // CPU_H