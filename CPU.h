#ifndef CPU_H
#define CPU_H

#include "Memory.h"

typedef struct cpu{
    byte A;
    byte X;
    byte Y;
    word PC;
    byte SP;
    byte Flags;
    
    size_t 	Cycles; 
} CPU;

void CPU_dump(CPU* cpu, FILE* stream);
bool CPU_offFlag(CPU* cpu, char flag);
bool CPU_onFlag(CPU* cpu, char flag);
bool CPU_getFlag(CPU* cpu, char flag);
void CPU_init(CPU* cpu);

void CPU_execute(CPU* cpu, Memory* mem);

// instructions:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define NUMBER_OF_INSTRUCTIONS 0xff

typedef void (*instruction)(CPU*, Memory*);

// LDA:
void instruction_LDA_imm(CPU* cpu, Memory* mem);
void instruction_LDA_abs(CPU* cpu, Memory* mem);
void instruction_LDA_abs_X(CPU* cpu, Memory* mem);
void instruction_LDA_abs_Y(CPU* cpu, Memory* mem);
void instruction_LDA_zp(CPU* cpu, Memory* mem);
void instruction_LDA_zp_X(CPU* cpu, Memory* mem);
void instruction_LDA_indirect_X(CPU* cpu, Memory* mem);
void instruction_LDA_indirect_Y(CPU* cpu, Memory* mem);





static instruction instruction_table[NUMBER_OF_INSTRUCTIONS] = {
    // LDA
    [0xa9] = instruction_LDA_imm,
    [0xad] = instruction_LDA_abs,
    [0xbd] = instruction_LDA_abs_X,
    [0xb9] = instruction_LDA_abs_Y,
    [0xa5] = instruction_LDA_zp,
    [0xb5] = instruction_LDA_zp_X,
    [0xa1] = instruction_LDA_indirect_X,
    [0xb1] = instruction_LDA_indirect_Y
};

#endif // CPU_H