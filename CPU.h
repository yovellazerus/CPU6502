#ifndef CPU_H
#define CPU_H

#include "Memory.h"

#define IS_SIGN_BYTE(ARG) ((ARG & 0b10000000) != 0)

#define RESET_P_REGISTER 0x34 // 0b00110100

#define Undefined_byte() (rand() / 256)

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

void CPU_execute(CPU* cpu, Memory* memory);
void CPU_reset(CPU* cpu, Memory* memory);
void CPU_tick(CPU* cpu, size_t amount);
void CPU_invalid_opcode(CPU* cpu, byte opcode);

// operations:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define NUMBER_OF_POSSIBLE_OPERATIONS (0xff + 1)

typedef void (*operation)(CPU*, Memory*);

// Load Operations:
void operation_LDA_Immediate(CPU* cpu, Memory* memory);
void operation_LDA_Absolute(CPU* cpu, Memory* memory);
void operation_LDA_Absolute_X(CPU* cpu, Memory* memory);
void operation_LDA_Absolute_Y(CPU* cpu, Memory* memory);
void operation_LDA_Zero_Page(CPU* cpu, Memory* memory);
void operation_LDA_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_LDA_Indirect_X(CPU* cpu, Memory* memory);
void operation_LDA_Indirect_Y(CPU* cpu, Memory* memory);

void operation_LDX_Immediate(CPU* cpu, Memory* memory);
void operation_LDX_Absolute(CPU* cpu, Memory* memory);
void operation_LDX_Absolute_Y(CPU* cpu, Memory* memory);
void operation_LDX_Zero_Page(CPU* cpu, Memory* memory);
void operation_LDX_Zero_Page_Y(CPU* cpu, Memory* memory);

void operation_LDY_Immediate(CPU* cpu, Memory* memory);
void operation_LDY_Absolute(CPU* cpu, Memory* memory);
void operation_LDY_Absolute_X(CPU* cpu, Memory* memory);
void operation_LDY_Zero_Page(CPU* cpu, Memory* memory);
void operation_LDY_Zero_Page_X(CPU* cpu, Memory* memory);

// Store Operations:
void operation_STA_Absolute(CPU* cpu, Memory* memory);
void operation_STA_Zero_Page(CPU* cpu, Memory* memory);
void operation_STA_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_STA_Absolute_X(CPU* cpu, Memory* memory);
void operation_STA_Absolute_Y(CPU* cpu, Memory* memory);
void operation_STA_Indirect_X(CPU* cpu, Memory* memory);
void operation_STA_Indirect_Y(CPU* cpu, Memory* memory);

void operation_STX_Zero_Page(CPU* cpu, Memory* memory);
void operation_STX_Zero_Page_Y(CPU* cpu, Memory* memory);
void operation_STX_Absolute(CPU* cpu, Memory* memory);

void operation_STY_Zero_Page(CPU* cpu, Memory* memory);
void operation_STY_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_STY_Absolute(CPU* cpu, Memory* memory);

// Register Transfers:
void operation_TAX_Implied(CPU* cpu, Memory* memory);
void operation_TAY_Implied(CPU* cpu, Memory* memory);
void operation_TXA_Implied(CPU* cpu, Memory* memory);
void operation_TYA_Implied(CPU* cpu, Memory* memory);

// Stack Operations:
void operation_PHA_Implied(CPU* cpu, Memory* memory);
void operation_PLA_Implied(CPU* cpu, Memory* memory);

// Logical:
void operation_AND_Immediate(CPU* cpu, Memory* memory);
void operation_AND_Zero_Page(CPU* cpu, Memory* memory);
void operation_AND_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_AND_Absolute(CPU* cpu, Memory* memory);
void operation_AND_Absolute_X(CPU* cpu, Memory* memory);
void operation_AND_Absolute_Y(CPU* cpu, Memory* memory);
void operation_AND_Indirect_X(CPU* cpu, Memory* memory);
void operation_AND_Indirect_Y(CPU* cpu, Memory* memory);

// Arithmetic:

// Increments & Decrements:

// Shifts:

// Jumps & Calls:
void operation_JMP_Absolute(CPU* cpu, Memory* memory);
void operation_JMP_Indirect(CPU* cpu, Memory* memory);
void operation_JSR_Absolute(CPU* cpu, Memory* memory);
void operation_RTS_Implied(CPU* cpu, Memory* memory);

// Branches:
void operation_BEQ_Relative(CPU* cpu, Memory* memory);

// Status Flag Changes:
void operation_CLC_Implied(CPU* cpu, Memory* memory);
void operation_CLD_Implied(CPU* cpu, Memory* memory);
void operation_CLI_Implied(CPU* cpu, Memory* memory);
void operation_CLV_Implied(CPU* cpu, Memory* memory);
void operation_SEC_Implied(CPU* cpu, Memory* memory);
void operation_SED_Implied(CPU* cpu, Memory* memory);
void operation_SEI_Implied(CPU* cpu, Memory* memory);

// System Functions:
void operation_BRK_Implied(CPU* cpu, Memory* memory);
void operation_NOP_Implied(CPU* cpu, Memory* memory);
void operation_RTI_Implied(CPU* cpu, Memory* memory);

// Unofficial not documented operations:
void operation_Unofficial_NOP_1A(CPU* cpu, Memory* memory);
void operation_Unofficial_NOP_3A(CPU* cpu, Memory* memory);
void operation_Unofficial_NOP_5A(CPU* cpu, Memory* memory);
void operation_Unofficial_NOP_7A(CPU* cpu, Memory* memory);
void operation_Unofficial_NOP_FA(CPU* cpu, Memory* memory);

// My operations:
// (There are none at the moment...)

static operation operation_table[NUMBER_OF_POSSIBLE_OPERATIONS] = {
    
    // Load Operations:
    [0xa9] = operation_LDA_Immediate,
    [0xa5] = operation_LDA_Zero_Page,
    [0xb5] = operation_LDA_Zero_Page_X,
    [0xad] = operation_LDA_Absolute,
    [0xbd] = operation_LDA_Absolute_X,
    [0xb9] = operation_LDA_Absolute_Y,
    [0xa1] = operation_LDA_Indirect_X,
    [0xb1] = operation_LDA_Indirect_Y,

    [0xa2] = operation_LDX_Immediate,
    [0xae] = operation_LDX_Absolute,
    [0xbe] = operation_LDX_Absolute_Y,
    [0xa6] = operation_LDX_Zero_Page,
    [0xb6] = operation_LDX_Zero_Page_Y,

    [0xa0] = operation_LDY_Immediate,
    [0xac] = operation_LDY_Absolute,
    [0xbc] = operation_LDY_Absolute_X,
    [0xa4] = operation_LDY_Zero_Page,
    [0xb4] = operation_LDY_Zero_Page_X,

    // Store Operations:
    [0x8d] = operation_STA_Absolute,
    [0x85] = operation_STA_Zero_Page,
    [0x95] = operation_STA_Zero_Page_X,
    [0x9d] = operation_STA_Absolute_X,
    [0x99] = operation_STA_Absolute_Y,
    [0x81] = operation_STA_Indirect_X,
    [0x91] = operation_STA_Indirect_Y,

    [0x86] = operation_STX_Zero_Page,
    [0x96] = operation_STX_Zero_Page_Y,
    [0x8e] = operation_STX_Absolute,

    [0x84] = operation_STY_Zero_Page,
    [0x94] = operation_STY_Zero_Page_X,
    [0x8c] = operation_STY_Absolute,

    // Register Transfers:
    [0xaa] = operation_TAX_Implied,
    [0xa8] = operation_TAY_Implied,
    [0x8a] = operation_TXA_Implied,
    [0x98] = operation_TYA_Implied,

    // Stack Operations:
    [0x48] = operation_PHA_Implied,
    [0x68] = operation_PLA_Implied,

    // Logical:
    [0x29] = operation_AND_Immediate,
    [0x25] = operation_AND_Zero_Page,
    [0x35] = operation_AND_Zero_Page_X,
    [0x2d] = operation_AND_Absolute,
    [0x3d] = operation_AND_Absolute_X,
    [0x39] = operation_AND_Absolute_Y,
    [0x21] = operation_AND_Indirect_X,
    [0x23] = operation_AND_Indirect_Y,

    // Arithmetic:

    // Increments & Decrements:

    // Shifts:

    // Jumps & Calls:
    [0x4c] = operation_JMP_Absolute,
    [0x6c] = operation_JMP_Indirect,
    [0x20] = operation_JSR_Absolute,
    [0x60] = operation_RTS_Implied,

    // Branches:
    [0xf0] = operation_BEQ_Relative,

    // Status Flag Changes:
    [0x18] = operation_CLC_Implied,
    [0xd8] = operation_CLD_Implied,
    [0x58] = operation_CLI_Implied,
    [0xb8] = operation_CLV_Implied,
    [0x38] = operation_SEC_Implied,
    [0xf8] = operation_SED_Implied,
    [0x78] = operation_SEI_Implied,

    // System Functions:
    [0xff] = operation_BRK_Implied, // TODO: in reality it is 0x00...modified for dubbing!  
    [0xea] = operation_NOP_Implied,
    [0x40] = operation_RTI_Implied,

    // Unofficial not documented operations:
    [0x1a] = operation_Unofficial_NOP_1A,
    [0x3a] = operation_Unofficial_NOP_3A,
    [0x5a] = operation_Unofficial_NOP_5A,
    [0x7a] = operation_Unofficial_NOP_7A,
    [0xfa] = operation_Unofficial_NOP_FA,

    // My operations:
    // (There are none at the moment...)
    
};

#endif // CPU_H