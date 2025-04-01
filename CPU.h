#ifndef CPU_H
#define CPU_H

#include "Memory.h"

#define IS_SIGN_BYTE(ARG) ((ARG & 0x80) != 0)
#define IS_6BIT_ON_BYTE(ARG) ((ARG & 0x40 != 0))

#define RESET_P_REGISTER 0x34 // 0b00110100

#define UNDEFINED_BYTE() (rand() / 256)

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

typedef enum{
    // Load Operations:
    ldai = 0xa9,
    ldaz = 0xa5,
    ldazx = 0xb5,
    ldaa = 0xad,
    ldaax = 0xbd,
    ldaay = 0xb9,
    ldaix = 0xa1,
    ldaiy = 0xb1,

    ldxi = 0xa2,
    ldxa = 0xae,
    ldxay = 0xbe,
    ldxz = 0xa6,
    ldxzy = 0xb6,

    ldyi = 0xa0,
    ldya = 0xac,
    ldyax = 0xbc,
    ldyz = 0xa4,
    ldyzx = 0xb4,

    // Store Operations:
    staa = 0x8d,
    staz = 0x85,
    stazx = 0x95,
    staax = 0x9d,
    staay = 0x99,
    staix = 0x81,
    staiy = 0x91,

    stxz = 0x86,
    stxzy = 0x96,
    stxa = 0x8e,

    styz = 0x84,
    styzx = 0x94,
    stya = 0x8c,

    // Register Transfers:
    tax = 0xaa,
    tay = 0xa8,
    txa = 0x8a,
    tya = 0x98,

    // Stack Operations:
    tsx = 0xba,
    txs = 0x9a,
    pha = 0x48,
    php = 0x08,
    pla = 0x68,
    plp = 0x28,

    // Logical:
    andi = 0x29,
    andz = 0x25,
    andzx = 0x35,
    anda = 0x2d,
    andax = 0x3d,
    anday = 0x39,
    andix = 0x21,
    andiy = 0x23,

    eori = 0x49,
    eorz = 0x45,
    eorzx = 0x55,
    eora = 0x4d,
    eorax = 0x5d,
    eoray = 0x59,
    eorix = 0x41,
    eoriy = 0x51,

    orai = 0x09,
    oraz = 0x05,
    orazx = 0x15,
    oraa = 0x0d,
    oraax = 0x1d,
    oraay = 0x19,
    oraix = 0x01,
    oraiy = 0x11,

    bitz = 0x24,
    bita = 0x2c,

    // Arithmetic:

    // Increments & Decrements:

    // Shifts:

    // Jumps & Calls:
    jmpa = 0x4c,
    jmpi = 0x6c,
    jsr = 0x20,
    rts = 0x60,

    // Branches:
    beq = 0xf0,

    // Status Flag Changes:
    clc = 0x18,
    cld = 0xd8,
    cli = 0x58,
    clv = 0xb8,
    sec = 0x38,
    sed = 0xf8,
    sei = 0x78,

    // System Functions:
    brk = 0xff, // TODO: in reality it is 0x00...modified for dubbing!  
    nop = 0xea,
    rti = 0x40,

    // Unofficial not documented operations:
    nop1a = 0x1a,
    nop3a = 0x3a,
    nop5a = 0x5a,
    nop7a = 0x7a,
    nopfa = 0xfa,

    // My operations:
    // (There are none at the moment...)
    
} OPCODE;

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
void operation_TSX_Implied(CPU* cpu, Memory* memory);
void operation_TXS_Implied(CPU* cpu, Memory* memory);
void operation_PHA_Implied(CPU* cpu, Memory* memory);
void operation_PHP_Implied(CPU* cpu, Memory* memory);
void operation_PLA_Implied(CPU* cpu, Memory* memory);
void operation_PLP_Implied(CPU* cpu, Memory* memory);

// Logical:
void operation_AND_Immediate(CPU* cpu, Memory* memory);
void operation_AND_Zero_Page(CPU* cpu, Memory* memory);
void operation_AND_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_AND_Absolute(CPU* cpu, Memory* memory);
void operation_AND_Absolute_X(CPU* cpu, Memory* memory);
void operation_AND_Absolute_Y(CPU* cpu, Memory* memory);
void operation_AND_Indirect_X(CPU* cpu, Memory* memory);
void operation_AND_Indirect_Y(CPU* cpu, Memory* memory);

void operation_EOR_Immediate(CPU* cpu, Memory* memory);
void operation_EOR_Zero_Page(CPU* cpu, Memory* memory);
void operation_EOR_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_EOR_Absolute(CPU* cpu, Memory* memory);
void operation_EOR_Absolute_X(CPU* cpu, Memory* memory);
void operation_EOR_Absolute_Y(CPU* cpu, Memory* memory);
void operation_EOR_Indirect_X(CPU* cpu, Memory* memory);
void operation_EOR_Indirect_Y(CPU* cpu, Memory* memory);

void operation_ORA_Immediate(CPU* cpu, Memory* memory);
void operation_ORA_Zero_Page(CPU* cpu, Memory* memory);
void operation_ORA_Zero_Page_X(CPU* cpu, Memory* memory);
void operation_ORA_Absolute(CPU* cpu, Memory* memory);
void operation_ORA_Absolute_X(CPU* cpu, Memory* memory);
void operation_ORA_Absolute_Y(CPU* cpu, Memory* memory);
void operation_ORA_Indirect_X(CPU* cpu, Memory* memory);
void operation_ORA_Indirect_Y(CPU* cpu, Memory* memory);

void operation_BIT_Zero_Page(CPU* cpu, Memory* memory);
void operation_BIT_Absolute(CPU* cpu, Memory* memory);

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
    [ldai] = operation_LDA_Immediate,
    [ldaz] = operation_LDA_Zero_Page,
    [ldazx] = operation_LDA_Zero_Page_X,
    [ldaa] = operation_LDA_Absolute,
    [ldaax] = operation_LDA_Absolute_X,
    [ldaay] = operation_LDA_Absolute_Y,
    [ldaix] = operation_LDA_Indirect_X,
    [ldaiy] = operation_LDA_Indirect_Y,

    [ldxi] = operation_LDX_Immediate,
    [ldxa] = operation_LDX_Absolute,
    [ldxay] = operation_LDX_Absolute_Y,
    [ldxz] = operation_LDX_Zero_Page,
    [ldxzy] = operation_LDX_Zero_Page_Y,

    [ldyi] = operation_LDY_Immediate,
    [ldya] = operation_LDY_Absolute,
    [ldyax] = operation_LDY_Absolute_X,
    [ldyz] = operation_LDY_Zero_Page,
    [ldyzx] = operation_LDY_Zero_Page_X,

    // Store Operations:
    [staa] = operation_STA_Absolute,
    [staz] = operation_STA_Zero_Page,
    [stazx] = operation_STA_Zero_Page_X,
    [staax] = operation_STA_Absolute_X,
    [staay] = operation_STA_Absolute_Y,
    [staix] = operation_STA_Indirect_X,
    [staiy] = operation_STA_Indirect_Y,

    [stxz] = operation_STX_Zero_Page,
    [stxzy] = operation_STX_Zero_Page_Y,
    [stxa] = operation_STX_Absolute,

    [styz] = operation_STY_Zero_Page,
    [styzx] = operation_STY_Zero_Page_X,
    [stya] = operation_STY_Absolute,

    // Register Transfers:
    [tax] = operation_TAX_Implied,
    [tay] = operation_TAY_Implied,
    [txa] = operation_TXA_Implied,
    [tya] = operation_TYA_Implied,

    // Stack Operations:
    [tsx] = operation_TSX_Implied,
    [txs] = operation_TXS_Implied,
    [pha] = operation_PHA_Implied,
    [php] = operation_PHP_Implied,
    [pla] = operation_PLA_Implied,
    [plp] = operation_PLP_Implied,

    // Logical:
    [andi] = operation_AND_Immediate,
    [andz] = operation_AND_Zero_Page,
    [andzx] = operation_AND_Zero_Page_X,
    [anda] = operation_AND_Absolute,
    [andax] = operation_AND_Absolute_X,
    [anday] = operation_AND_Absolute_Y,
    [andix] = operation_AND_Indirect_X,
    [andiy] = operation_AND_Indirect_Y,

    [eori] = operation_EOR_Immediate,
    [eorz] = operation_EOR_Zero_Page,
    [eorzx] = operation_EOR_Zero_Page_X,
    [eora] = operation_EOR_Absolute,
    [eorax] = operation_EOR_Absolute_X,
    [eoray] = operation_EOR_Absolute_Y,
    [eorix] = operation_EOR_Indirect_X,
    [eoriy] = operation_EOR_Indirect_Y,

    [orai] = operation_ORA_Immediate,
    [oraz] = operation_ORA_Zero_Page,
    [orazx] = operation_ORA_Zero_Page_X,
    [oraa] = operation_ORA_Absolute,
    [oraax] = operation_ORA_Absolute_X,
    [oraay] = operation_ORA_Absolute_Y,
    [oraix] = operation_ORA_Indirect_X,
    [oraiy] = operation_ORA_Indirect_Y,

    [bitz] = operation_BIT_Zero_Page,
    [bita] = operation_BIT_Absolute,

    // Arithmetic:

    // Increments & Decrements:

    // Shifts:

    // Jumps & Calls:
    [jmpa] = operation_JMP_Absolute,
    [jmpi] = operation_JMP_Indirect,
    [jsr] = operation_JSR_Absolute,
    [rts] = operation_RTS_Implied,

    // Branches:
    [beq] = operation_BEQ_Relative,

    // Status Flag Changes:
    [clc] = operation_CLC_Implied,
    [cld] = operation_CLD_Implied,
    [cli] = operation_CLI_Implied,
    [clv] = operation_CLV_Implied,
    [sec] = operation_SEC_Implied,
    [sed] = operation_SED_Implied,
    [sei] = operation_SEI_Implied,

    // System Functions:
    [brk] = operation_BRK_Implied,  
    [nop] = operation_NOP_Implied,
    [rti] = operation_RTI_Implied,

    // Unofficial not documented operations:
    [nop1a] = operation_Unofficial_NOP_1A,
    [nop3a] = operation_Unofficial_NOP_3A,
    [nop5a] = operation_Unofficial_NOP_5A,
    [nop7a] = operation_Unofficial_NOP_7A,
    [nopfa] = operation_Unofficial_NOP_FA,

    // My operations:
    // (There are none at the moment...)
    
};

#endif // CPU_H