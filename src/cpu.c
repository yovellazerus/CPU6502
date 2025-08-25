
#include "../include\cpu.h"

const char* opcode_to_cstr[0xff + 1] = {

    [Opcode_LDA_Immediate]   = "LDA_I",
    [Opcode_LDA_ZeroPage ]   = "LDA_Z",
    [Opcode_LDA_ZeroPageX]   = "LDA_ZX",
    [Opcode_LDA_Absolute ]   = "LDA_A",
    [Opcode_LDA_AbsoluteX]   = "LDA_AX",
    [Opcode_LDA_AbsoluteY]   = "LDA_AY",
    [Opcode_LDA_IndirectX]   = "LDA_IX",
    [Opcode_LDA_IndirectY]   = "LDA_IY",
    [Opcode_LDX_Immediate]   = "LDX_I",
    [Opcode_LDX_ZeroPage ]   = "LDX_Z",
    [Opcode_LDX_ZeroPageY]   = "LDX_ZY",
    [Opcode_LDX_Absolute ]   = "LDX_A",
    [Opcode_LDX_AbsoluteY]   = "LDX_AY",
    [Opcode_LDY_Immediate]   = "LDY_I",
    [Opcode_LDY_ZeroPage ]   = "LDY_Z",
    [Opcode_LDY_ZeroPageX]   = "LDY_ZX",
    [Opcode_LDY_Absolute ]   = "LDY_A",
    [Opcode_LDY_AbsoluteX]   = "LDY_AX",
    [Opcode_STA_ZeroPage ]   = "STA_Z",
    [Opcode_STA_ZeroPageX]   = "STA_ZX",
    [Opcode_STA_Absolute ]   = "STA_A",
    [Opcode_STA_AbsoluteX]   = "STA_AX",
    [Opcode_STA_AbsoluteY]   = "STA_AY",
    [Opcode_STA_IndirectX]   = "STA_IX",
    [Opcode_STA_IndirectY]   = "STA_IY",
    [Opcode_STX_ZeroPage ]   = "STX_Z",
    [Opcode_STX_ZeroPageY]   = "STX_ZY",
    [Opcode_STX_Absolute ]   = "STX_A",
    [Opcode_STY_ZeroPage ]   = "STY_Z",
    [Opcode_STY_ZeroPageX]   = "STY_ZX",
    [Opcode_STY_Absolute ]   = "STY_A",
    [Opcode_TAX]             = "TAX",
    [Opcode_TAY]             = "TAY",
    [Opcode_TSX]             = "TSX",
    [Opcode_TXA]             = "TXA",
    [Opcode_TXS]             = "TXS",
    [Opcode_TYA]             = "TYA",
    [Opcode_PHA]             = "PHA",
    [Opcode_PHP]             = "PHP",
    [Opcode_PLA]             = "PLA",
    [Opcode_PLP]             = "PLP",
    [Opcode_AND_Immediate]   = "AND_I",
    [Opcode_AND_ZeroPage ]   = "AND_Z",
    [Opcode_AND_ZeroPageX]   = "AND_ZX",
    [Opcode_AND_Absolute ]   = "AND_A",
    [Opcode_AND_AbsoluteX]   = "AND_AX",
    [Opcode_AND_AbsoluteY]   = "AND_AY",
    [Opcode_AND_IndirectX]   = "AND_IX",
    [Opcode_AND_IndirectY]   = "AND_IY",
    [Opcode_EOR_Immediate]   = "EOR_I",
    [Opcode_EOR_ZeroPage ]   = "EOR_Z",
    [Opcode_EOR_ZeroPageX]   = "EOR_ZX",
    [Opcode_EOR_Absolute ]   = "EOR_A",
    [Opcode_EOR_AbsoluteX]   = "EOR_AX",
    [Opcode_EOR_AbsoluteY]   = "EOR_AY",
    [Opcode_EOR_IndirectX]   = "EOR_IX",
    [Opcode_EOR_IndirectY]   = "EOR_IY",
    [Opcode_ORA_Immediate]   = "ORA_I",
    [Opcode_ORA_ZeroPage ]   = "ORA_Z",
    [Opcode_ORA_ZeroPageX]   = "ORA_ZX",
    [Opcode_ORA_Absolute ]   = "ORA_A",
    [Opcode_ORA_AbsoluteX]   = "ORA_AX",
    [Opcode_ORA_AbsoluteY]   = "ORA_AY",
    [Opcode_ORA_IndirectX]   = "ORA_IX",
    [Opcode_ORA_IndirectY]   = "ORA_IY",
    [Opcode_BIT_ZeroPage ]   = "BIT_Z",
    [Opcode_BIT_Absolute ]   = "BIT_A",
    [Opcode_ADC_Immediate]   = "ADC_I",
    [Opcode_ADC_ZeroPage ]   = "ADC_Z",
    [Opcode_ADC_ZeroPageX]   = "ADC_ZX",
    [Opcode_ADC_Absolute ]   = "ADC_A",
    [Opcode_ADC_AbsoluteX]   = "ADC_AX",
    [Opcode_ADC_AbsoluteY]   = "ADC_AY",
    [Opcode_ADC_IndirectX]   = "ADC_IX",
    [Opcode_ADC_IndirectY]   = "ADC_IY",
    [Opcode_SBC_Immediate]   = "SBC_I",
    [Opcode_SBC_ZeroPage ]   = "SBC_Z",
    [Opcode_SBC_ZeroPageX]   = "SBC_ZX",
    [Opcode_SBC_Absolute ]   = "SBC_A",
    [Opcode_SBC_AbsoluteX]   = "SBC_AX",
    [Opcode_SBC_AbsoluteY]   = "SBC_AY",
    [Opcode_SBC_IndirectX]   = "SBC_IX",
    [Opcode_SBC_IndirectY]   = "SBC_IY",
    [Opcode_CMP_Immediate]   = "CMP_I",
    [Opcode_CMP_ZeroPage ]   = "CMP_Z",
    [Opcode_CMP_ZeroPageX]   = "CMP_ZX",
    [Opcode_CMP_Absolute ]   = "CMP_A",
    [Opcode_CMP_AbsoluteX]   = "CMP_AX",
    [Opcode_CMP_AbsoluteY]   = "CMP_AY",
    [Opcode_CMP_IndirectX]   = "CMP_IX",
    [Opcode_CMP_IndirectY]   = "CMP_I",
    [Opcode_CPX_Immediate]   = "CPX_I",
    [Opcode_CPX_ZeroPage ]   = "CPX_Z",
    [Opcode_CPX_Absolute ]   = "CPX_A",
    [Opcode_CPY_Immediate]   = "CPY_I",
    [Opcode_CPY_ZeroPage ]   = "CPY_Z",
    [Opcode_CPY_Absolute ]   = "CPY_A",
    [Opcode_INC_ZeroPage ]   = "INC_Z",
    [Opcode_INC_ZeroPageX]   = "INC_ZX",
    [Opcode_INC_Absolute ]   = "INC_A",
    [Opcode_INC_AbsoluteX]   = "INC_A",
    [Opcode_INX            ] = "INX",
    [Opcode_INY            ] = "INY",
    [Opcode_DEC_ZeroPage   ] = "DEC_Z",
    [Opcode_DEC_ZeroPageX  ] = "DEC_ZX",
    [Opcode_DEC_Absolute   ] = "DEC_A",
    [Opcode_DEC_AbsoluteX  ] = "DEC_AX",
    [Opcode_DEX            ] = "DEX",
    [Opcode_DEY            ] = "DEY",
    [Opcode_ASL_Accumulator] = "ASL_A",
    [Opcode_ASL_ZeroPage   ] = "ASL_Z",
    [Opcode_ASL_ZeroPageX  ] = "ASL_ZX",
    [Opcode_ASL_Absolute   ] = "ASL_A",
    [Opcode_ASL_AbsoluteX  ] = "ASL_A",
    [Opcode_LSR_Accumulator] = "LSR_A",
    [Opcode_LSR_ZeroPage   ] = "LSR_Z",
    [Opcode_LSR_ZeroPageX  ] = "LSR_ZX",
    [Opcode_LSR_Absolute   ] = "LSR_A",
    [Opcode_LSR_AbsoluteX  ] = "LSR_AX",
    [Opcode_ROL_Accumulator] = "ROL_A",
    [Opcode_ROL_ZeroPage   ] = "ROL_Z",
    [Opcode_ROL_ZeroPageX  ] = "ROL_ZX",
    [Opcode_ROL_Absolute   ] = "ROL_A",
    [Opcode_ROL_AbsoluteX  ] = "ROL_AX",
    [Opcode_ROR_Accumulator] = "ROR_A",
    [Opcode_ROR_ZeroPage   ] = "ROR_Z",
    [Opcode_ROR_ZeroPageX  ] = "ROR_ZX",
    [Opcode_ROR_Absolute   ] = "ROR_A",
    [Opcode_ROR_AbsoluteX  ] = "ROR_AX",
    [Opcode_JMP_Absolute   ] = "JMP_A",
    [Opcode_JMP_Indirect   ] = "JMP_I",
    [Opcode_JSR            ] = "JSR",
    [Opcode_RTS            ] = "RTS",
    [Opcode_BCC            ] = "BCC",
    [Opcode_BCS            ] = "BCS",
    [Opcode_BEQ            ] = "BEQ",
    [Opcode_BMI            ] = "BMI",
    [Opcode_BNE            ] = "BNE",
    [Opcode_BPL            ] = "BPL",
    [Opcode_BVC            ] = "BVC",
    [Opcode_BVS            ] = "BVS",
    [Opcode_CLC            ] = "CLC",
    [Opcode_CLD            ] = "CLD",
    [Opcode_CLI            ] = "CLI",
    [Opcode_CLV            ] = "CLV",
    [Opcode_SEC            ] = "SEC",
    [Opcode_SED            ] = "SED",
    [Opcode_SEI            ] = "SEI",
    [Opcode_BRK            ] = "BRK",
    [Opcode_NOP            ] = "NOP",
    [Opcode_RTI            ] = "RTI",
};

size_t opcode_to_numberOperands[0xff + 1] = {
    // LDA
    [Opcode_LDA_Immediate]   = 1,
    [Opcode_LDA_ZeroPage]    = 1,
    [Opcode_LDA_ZeroPageX]   = 1,
    [Opcode_LDA_Absolute]    = 2,
    [Opcode_LDA_AbsoluteX]   = 2,
    [Opcode_LDA_AbsoluteY]   = 2,
    [Opcode_LDA_IndirectX]   = 1,
    [Opcode_LDA_IndirectY]   = 1,

    // LDX
    [Opcode_LDX_Immediate]   = 1,
    [Opcode_LDX_ZeroPage]    = 1,
    [Opcode_LDX_ZeroPageY]   = 1,
    [Opcode_LDX_Absolute]    = 2,
    [Opcode_LDX_AbsoluteY]   = 2,

    // LDY
    [Opcode_LDY_Immediate]   = 1,
    [Opcode_LDY_ZeroPage]    = 1,
    [Opcode_LDY_ZeroPageX]   = 1,
    [Opcode_LDY_Absolute]    = 2,
    [Opcode_LDY_AbsoluteX]   = 2,

    // STA
    [Opcode_STA_ZeroPage]    = 1,
    [Opcode_STA_ZeroPageX]   = 1,
    [Opcode_STA_Absolute]    = 2,
    [Opcode_STA_AbsoluteX]   = 2,
    [Opcode_STA_AbsoluteY]   = 2,
    [Opcode_STA_IndirectX]   = 1,
    [Opcode_STA_IndirectY]   = 1,

    // STX
    [Opcode_STX_ZeroPage]    = 1,
    [Opcode_STX_ZeroPageY]   = 1,
    [Opcode_STX_Absolute]    = 2,

    // STY
    [Opcode_STY_ZeroPage]    = 1,
    [Opcode_STY_ZeroPageX]   = 1,
    [Opcode_STY_Absolute]    = 2,

    // Transfer / stack / implied
    [Opcode_TAX]             = 0,
    [Opcode_TAY]             = 0,
    [Opcode_TSX]             = 0,
    [Opcode_TXA]             = 0,
    [Opcode_TXS]             = 0,
    [Opcode_TYA]             = 0,
    [Opcode_PHA]             = 0,
    [Opcode_PHP]             = 0,
    [Opcode_PLA]             = 0,
    [Opcode_PLP]             = 0,
    [Opcode_CLC]             = 0,
    [Opcode_CLD]             = 0,
    [Opcode_CLI]             = 0,
    [Opcode_CLV]             = 0,
    [Opcode_SEC]             = 0,
    [Opcode_SED]             = 0,
    [Opcode_SEI]             = 0,
    [Opcode_NOP]             = 0,
    [Opcode_BRK]             = 0,
    [Opcode_RTS]             = 0,
    [Opcode_RTI]             = 0,

    // JMP / JSR
    [Opcode_JMP_Absolute]    = 2,
    [Opcode_JMP_Indirect]    = 2,
    [Opcode_JSR]             = 2,

    // Branches (relative)
    [Opcode_BCC]             = 1,
    [Opcode_BCS]             = 1,
    [Opcode_BEQ]             = 1,
    [Opcode_BMI]             = 1,
    [Opcode_BNE]             = 1,
    [Opcode_BPL]             = 1,
    [Opcode_BVC]             = 1,
    [Opcode_BVS]             = 1,

    // AND
    [Opcode_AND_Immediate]   = 1,
    [Opcode_AND_ZeroPage]    = 1,
    [Opcode_AND_ZeroPageX]   = 1,
    [Opcode_AND_Absolute]    = 2,
    [Opcode_AND_AbsoluteX]   = 2,
    [Opcode_AND_AbsoluteY]   = 2,
    [Opcode_AND_IndirectX]   = 1,
    [Opcode_AND_IndirectY]   = 1,

    // ORA
    [Opcode_ORA_Immediate]   = 1,
    [Opcode_ORA_ZeroPage]    = 1,
    [Opcode_ORA_ZeroPageX]   = 1,
    [Opcode_ORA_Absolute]    = 2,
    [Opcode_ORA_AbsoluteX]   = 2,
    [Opcode_ORA_AbsoluteY]   = 2,
    [Opcode_ORA_IndirectX]   = 1,
    [Opcode_ORA_IndirectY]   = 1,

    // EOR
    [Opcode_EOR_Immediate]   = 1,
    [Opcode_EOR_ZeroPage]    = 1,
    [Opcode_EOR_ZeroPageX]   = 1,
    [Opcode_EOR_Absolute]    = 2,
    [Opcode_EOR_AbsoluteX]   = 2,
    [Opcode_EOR_AbsoluteY]   = 2,
    [Opcode_EOR_IndirectX]   = 1,
    [Opcode_EOR_IndirectY]   = 1,

    // ADC
    [Opcode_ADC_Immediate]   = 1,
    [Opcode_ADC_ZeroPage]    = 1,
    [Opcode_ADC_ZeroPageX]   = 1,
    [Opcode_ADC_Absolute]    = 2,
    [Opcode_ADC_AbsoluteX]   = 2,
    [Opcode_ADC_AbsoluteY]   = 2,
    [Opcode_ADC_IndirectX]   = 1,
    [Opcode_ADC_IndirectY]   = 1,

    // SBC
    [Opcode_SBC_Immediate]   = 1,
    [Opcode_SBC_ZeroPage]    = 1,
    [Opcode_SBC_ZeroPageX]   = 1,
    [Opcode_SBC_Absolute]    = 2,
    [Opcode_SBC_AbsoluteX]   = 2,
    [Opcode_SBC_AbsoluteY]   = 2,
    [Opcode_SBC_IndirectX]   = 1,
    [Opcode_SBC_IndirectY]   = 1,

    // CMP
    [Opcode_CMP_Immediate]   = 1,
    [Opcode_CMP_ZeroPage]    = 1,
    [Opcode_CMP_ZeroPageX]   = 1,
    [Opcode_CMP_Absolute]    = 2,
    [Opcode_CMP_AbsoluteX]   = 2,
    [Opcode_CMP_AbsoluteY]   = 2,
    [Opcode_CMP_IndirectX]   = 1,
    [Opcode_CMP_IndirectY]   = 1,

    // CPX
    [Opcode_CPX_Immediate]   = 1,
    [Opcode_CPX_ZeroPage]    = 1,
    [Opcode_CPX_Absolute]    = 2,

    // CPY
    [Opcode_CPY_Immediate]   = 1,
    [Opcode_CPY_ZeroPage]    = 1,
    [Opcode_CPY_Absolute]    = 2,

    // INC / INX / INY
    [Opcode_INC_ZeroPage]    = 1,
    [Opcode_INC_ZeroPageX]   = 1,
    [Opcode_INC_Absolute]    = 2,
    [Opcode_INC_AbsoluteX]   = 2,
    [Opcode_INX]             = 0,
    [Opcode_INY]             = 0,

    // DEC / DEX / DEY
    [Opcode_DEC_ZeroPage]    = 1,
    [Opcode_DEC_ZeroPageX]   = 1,
    [Opcode_DEC_Absolute]    = 2,
    [Opcode_DEC_AbsoluteX]   = 2,
    [Opcode_DEX]             = 0,
    [Opcode_DEY]             = 0,

    // Shift / Rotate
    [Opcode_ASL_Accumulator] = 0,
    [Opcode_ASL_ZeroPage]    = 1,
    [Opcode_ASL_ZeroPageX]   = 1,
    [Opcode_ASL_Absolute]    = 2,
    [Opcode_ASL_AbsoluteX]   = 2,
    [Opcode_LSR_Accumulator] = 0,
    [Opcode_LSR_ZeroPage]    = 1,
    [Opcode_LSR_ZeroPageX]   = 1,
    [Opcode_LSR_Absolute]    = 2,
    [Opcode_LSR_AbsoluteX]   = 2,
    [Opcode_ROL_Accumulator] = 0,
    [Opcode_ROL_ZeroPage]    = 1,
    [Opcode_ROL_ZeroPageX]   = 1,
    [Opcode_ROL_Absolute]    = 2,
    [Opcode_ROL_AbsoluteX]   = 2,
    [Opcode_ROR_Accumulator] = 0,
    [Opcode_ROR_ZeroPage]    = 1,
    [Opcode_ROR_ZeroPageX]   = 1,
    [Opcode_ROR_Absolute]    = 2,
    [Opcode_ROR_AbsoluteX]   = 2,

    // BIT
    [Opcode_BIT_ZeroPage]    = 1,
    [Opcode_BIT_Absolute]    = 2,
};

Addressing_mode opcode_to_Addressing_mode[0xff + 1] = {

    [Opcode_LDA_Immediate]   = Add_Immediate,
    [Opcode_LDA_ZeroPage ]   = Add_ZeroPage,
    [Opcode_LDA_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_LDA_Absolute ]   = Add_Absolute,
    [Opcode_LDA_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_LDA_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_LDA_IndirectX]   = Add_IndirectX,
    [Opcode_LDA_IndirectY]   = Add_IndirectY,
    [Opcode_LDX_Immediate]   = Add_Immediate,
    [Opcode_LDX_ZeroPage ]   = Add_ZeroPage,
    [Opcode_LDX_ZeroPageY]   = Add_ZeroPageY,
    [Opcode_LDX_Absolute ]   = Add_Absolute,
    [Opcode_LDX_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_LDY_Immediate]   = Add_Immediate,
    [Opcode_LDY_ZeroPage ]   = Add_ZeroPage,
    [Opcode_LDY_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_LDY_Absolute ]   = Add_Absolute,
    [Opcode_LDY_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_STA_ZeroPage ]   = Add_ZeroPage,
    [Opcode_STA_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_STA_Absolute ]   = Add_Absolute,
    [Opcode_STA_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_STA_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_STA_IndirectX]   = Add_IndirectX,
    [Opcode_STA_IndirectY]   = Add_IndirectY,
    [Opcode_STX_ZeroPage ]   = Add_ZeroPage,
    [Opcode_STX_ZeroPageY]   = Add_ZeroPageY,
    [Opcode_STX_Absolute ]   = Add_Absolute,
    [Opcode_STY_ZeroPage ]   = Add_ZeroPage,
    [Opcode_STY_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_STY_Absolute ]   = Add_Absolute,
    [Opcode_TAX]             = Add_Implied    ,  
    [Opcode_TAY]             = Add_Implied    ,  
    [Opcode_TSX]             = Add_Implied    ,  
    [Opcode_TXA]             = Add_Implied    ,  
    [Opcode_TXS]             = Add_Implied    ,  
    [Opcode_TYA]             = Add_Implied    ,  
    [Opcode_PHA]             = Add_Implied    ,  
    [Opcode_PHP]             = Add_Implied    ,  
    [Opcode_PLA]             = Add_Implied    ,  
    [Opcode_PLP]             = Add_Implied    ,  
    [Opcode_AND_Immediate]   = Add_Immediate,
    [Opcode_AND_ZeroPage ]   = Add_ZeroPage,
    [Opcode_AND_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_AND_Absolute ]   = Add_Absolute,
    [Opcode_AND_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_AND_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_AND_IndirectX]   = Add_IndirectX,
    [Opcode_AND_IndirectY]   = Add_IndirectY,
    [Opcode_EOR_Immediate]   = Add_Immediate,
    [Opcode_EOR_ZeroPage ]   = Add_ZeroPage,
    [Opcode_EOR_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_EOR_Absolute ]   = Add_Absolute,
    [Opcode_EOR_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_EOR_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_EOR_IndirectX]   = Add_IndirectX,
    [Opcode_EOR_IndirectY]   = Add_IndirectY,
    [Opcode_ORA_Immediate]   = Add_Immediate,
    [Opcode_ORA_ZeroPage ]   = Add_ZeroPage,
    [Opcode_ORA_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_ORA_Absolute ]   = Add_Absolute,
    [Opcode_ORA_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_ORA_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_ORA_IndirectX]   = Add_IndirectX,
    [Opcode_ORA_IndirectY]   = Add_IndirectY,
    [Opcode_BIT_ZeroPage ]   = Add_ZeroPage,
    [Opcode_BIT_Absolute ]   = Add_Absolute,
    [Opcode_ADC_Immediate]   = Add_Immediate,
    [Opcode_ADC_ZeroPage ]   = Add_ZeroPage,
    [Opcode_ADC_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_ADC_Absolute ]   = Add_Absolute,
    [Opcode_ADC_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_ADC_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_ADC_IndirectX]   = Add_IndirectX,
    [Opcode_ADC_IndirectY]   = Add_IndirectY,
    [Opcode_SBC_Immediate]   = Add_Immediate,
    [Opcode_SBC_ZeroPage ]   = Add_ZeroPage,
    [Opcode_SBC_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_SBC_Absolute ]   = Add_Absolute,
    [Opcode_SBC_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_SBC_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_SBC_IndirectX]   = Add_IndirectX,
    [Opcode_SBC_IndirectY]   = Add_IndirectY,
    [Opcode_CMP_Immediate]   = Add_Immediate,
    [Opcode_CMP_ZeroPage ]   = Add_ZeroPage,
    [Opcode_CMP_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_CMP_Absolute ]   = Add_Absolute,
    [Opcode_CMP_AbsoluteX]   = Add_AbsoluteX,
    [Opcode_CMP_AbsoluteY]   = Add_AbsoluteY,
    [Opcode_CMP_IndirectX]   = Add_IndirectX,
    [Opcode_CMP_IndirectY]   = Add_Indirect,
    [Opcode_CPX_Immediate]   = Add_Immediate,
    [Opcode_CPX_ZeroPage ]   = Add_ZeroPage,
    [Opcode_CPX_Absolute ]   = Add_Absolute,
    [Opcode_CPY_Immediate]   = Add_Immediate,
    [Opcode_CPY_ZeroPage ]   = Add_ZeroPage,
    [Opcode_CPY_Absolute ]   = Add_Absolute,
    [Opcode_INC_ZeroPage ]   = Add_ZeroPage,
    [Opcode_INC_ZeroPageX]   = Add_ZeroPageX,
    [Opcode_INC_Absolute ]   = Add_Absolute,
    [Opcode_INC_AbsoluteX]   = Add_Absolute,
    [Opcode_INX            ] = Add_Implied    ,  
    [Opcode_INY            ] = Add_Implied    ,  
    [Opcode_DEC_ZeroPage   ] = Add_ZeroPage,
    [Opcode_DEC_ZeroPageX  ] = Add_ZeroPageX,
    [Opcode_DEC_Absolute   ] = Add_Absolute,
    [Opcode_DEC_AbsoluteX  ] = Add_AbsoluteX,
    [Opcode_DEX            ] = Add_Implied    ,  
    [Opcode_DEY            ] = Add_Implied    ,  
    [Opcode_ASL_Accumulator] = Add_Accumulator,
    [Opcode_ASL_ZeroPage   ] = Add_ZeroPage,
    [Opcode_ASL_ZeroPageX  ] = Add_ZeroPageX,
    [Opcode_ASL_Absolute   ] = Add_Absolute,
    [Opcode_ASL_AbsoluteX  ] = Add_Absolute,
    [Opcode_LSR_Accumulator] = Add_Accumulator,
    [Opcode_LSR_ZeroPage   ] = Add_ZeroPage,
    [Opcode_LSR_ZeroPageX  ] = Add_ZeroPageX,
    [Opcode_LSR_Absolute   ] = Add_Absolute,
    [Opcode_LSR_AbsoluteX  ] = Add_AbsoluteX,
    [Opcode_ROL_Accumulator] = Add_Accumulator,
    [Opcode_ROL_ZeroPage   ] = Add_ZeroPage,
    [Opcode_ROL_ZeroPageX  ] = Add_ZeroPageX,
    [Opcode_ROL_Absolute   ] = Add_Absolute,
    [Opcode_ROL_AbsoluteX  ] = Add_AbsoluteX,
    [Opcode_ROR_Accumulator] = Add_Accumulator,
    [Opcode_ROR_ZeroPage   ] = Add_ZeroPage,
    [Opcode_ROR_ZeroPageX  ] = Add_ZeroPageX,
    [Opcode_ROR_Absolute   ] = Add_Absolute,
    [Opcode_ROR_AbsoluteX  ] = Add_AbsoluteX,
    [Opcode_JMP_Absolute   ] = Add_Absolute,
    [Opcode_JMP_Indirect   ] = Add_Indirect,
    [Opcode_JSR            ] = Add_Implied    ,       
    [Opcode_RTS            ] = Add_Implied    ,       
    [Opcode_BCC            ] = Add_Relative   ,        
    [Opcode_BCS            ] = Add_Relative   ,        
    [Opcode_BEQ            ] = Add_Relative   ,        
    [Opcode_BMI            ] = Add_Relative   ,        
    [Opcode_BNE            ] = Add_Relative   ,        
    [Opcode_BPL            ] = Add_Relative   ,        
    [Opcode_BVC            ] = Add_Relative   ,        
    [Opcode_BVS            ] = Add_Relative   ,        
    [Opcode_CLC            ] = Add_Implied    ,       
    [Opcode_CLD            ] = Add_Implied    ,       
    [Opcode_CLI            ] = Add_Implied    ,       
    [Opcode_CLV            ] = Add_Implied    ,       
    [Opcode_SEC            ] = Add_Implied    ,       
    [Opcode_SED            ] = Add_Implied    ,       
    [Opcode_SEI            ] = Add_Implied    ,       
    [Opcode_BRK            ] = Add_Brk        ,   
    [Opcode_NOP            ] = Add_Implied    ,       
    [Opcode_RTI            ] = Add_Implied    ,       
};


void CPU_dump_cpu(CPU * cpu, FILE * file)
{
    fprintf(file, "CPU: {\n");
    fprintf(file, "A: %d (0x%.2x)\n", cpu->A, cpu->A);
    fprintf(file, "X: %d (0x%.2x)\n", cpu->X, cpu->X);
    fprintf(file, "Y: %d (0x%.2x)\n", cpu->Y, cpu->Y);
    fprintf(file, "PC: 0x%.4x\n", cpu->PC);
    fprintf(file, "SP: 0x%.2x\n", cpu->SP);
    fprintf(file, "P {\n");
    fprintf(file, "    n: %d\n", CPU_getFlag(cpu, 'n'));
    fprintf(file, "    v: %d\n", CPU_getFlag(cpu, 'v'));
    fprintf(file, "    u: %d\n", CPU_getFlag(cpu, 'u'));
    fprintf(file, "    b: %d\n", CPU_getFlag(cpu, 'b'));
    fprintf(file, "    d: %d\n", CPU_getFlag(cpu, 'd'));
    fprintf(file, "    i: %d\n", CPU_getFlag(cpu, 'i'));
    fprintf(file, "    z: %d\n", CPU_getFlag(cpu, 'z'));
    fprintf(file, "    c: %d\n", CPU_getFlag(cpu, 'c'));
    fprintf(file, "  }\n");
    fprintf(file, "}\n");
}

void CPU_dump_memory(CPU * cpu, FILE * file)
{
    if(!file) file = stdout;
    word line = 0;
    for(int i = 0; i < MEMORY_SIZE; i++){
        if(i % 8 == 0){
            fprintf(file, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(file, " 0x%.2x ", cpu->memory[i]);
    }
}

void CPU_dumpProgram(CPU* cpu, word entry_point, size_t program_size, FILE* file){
    if(!file) file = stdout;
    for(word addr = entry_point; addr <  program_size + entry_point; addr++){
        if(opcode_to_numberOperands[cpu->memory[addr]] == 0){
            fprintf(file, "0x%.4x:\t%s\n", addr, 
                opcode_to_cstr[cpu->memory[addr]] ? opcode_to_cstr[cpu->memory[addr]] : "???");
        }
        else if(opcode_to_numberOperands[cpu->memory[addr]] == 1){
            fprintf(file, "0x%.4x:\t%s", addr, 
                opcode_to_cstr[cpu->memory[addr]] ? opcode_to_cstr[cpu->memory[addr]] : "???");
            addr++;
            fprintf(file, "\t%d ($%.2x)\n", cpu->memory[addr], cpu->memory[addr]);
        }
        else if(opcode_to_numberOperands[cpu->memory[addr]] == 2){
            fprintf(file, "0x%.4x:\t%s", addr, 
                opcode_to_cstr[cpu->memory[addr]] ? opcode_to_cstr[cpu->memory[addr]] : "???");
            addr += 2;
            fprintf(file, "\t$%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x\n", cpu->memory[addr]);
            addr++;
        }
        else{
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: operand number (%zx) more than 2 for opcode: %s\n",
                opcode_to_numberOperands[cpu->memory[addr]],
                opcode_to_cstr[cpu->memory[addr]]
            );
            set_color(COLOR_RESET, stderr);
        }
    }
}

void CPU_dump_stack(CPU * cpu, FILE * file)
{
    if(!file) file = stdout;
    for(int i = STACK_END; i >= STACK_START; i--){
        fprintf(file, "0x%.4x: ", i);
        fprintf(file, " 0x%.2x", cpu->memory[i]);
        if(i == cpu->SP + STACK_START){
            fprintf(file, " <--- sp\n");
        }
        else{
            fprintf(file, "\n");
        }
    }
}

bool CPU_getFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            return (1 & (cpu->P >> 7));
        case 'v':
            return (1 & (cpu->P >> 6));
        case 'u': // undefine, on all the time
            return (1 & (cpu->P >> 5));
        case 'b':
            return (1 & (cpu->P >> 4));
        case 'd':
            return (1 & (cpu->P >> 3));
        case 'i':
            return (1 & (cpu->P >> 2));
        case 'z':
            return (1 & (cpu->P >> 1));
        case 'c':
            return (1 & (cpu->P >> 0));
        default:
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            set_color(COLOR_RESET, stderr);
            return false;
    }
}

void CPU_push(CPU * cpu, char reg)
{
    switch (reg)
    {
    case 'A':
        cpu->memory[STACK_START + cpu->SP--] = cpu->A;
        break;
    case 'X':
        cpu->memory[STACK_START + cpu->SP--] = cpu->X;
        break;
    case 'Y':
        cpu->memory[STACK_START + cpu->SP--] = cpu->Y;
        break;
    case 'P':
        cpu->memory[STACK_START + cpu->SP--] = cpu->P;
        break;
    case 'C': // for PC
        cpu->memory[STACK_START + cpu->SP--] = LOW_BYTE(cpu->PC);
        cpu->memory[STACK_START + cpu->SP--] = HIGH_BYTE(cpu->PC);
        break;
    default:
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "ERROR: non legal push operation for register `%c`\n", reg);
        set_color(COLOR_RESET, stderr);
        break;
    }
}

void CPU_pop(CPU * cpu, char reg)
{
    word addr = 0;
    byte low = 0;
    byte high = 0;

    switch (reg)
    {
    case 'A':
        cpu->A = cpu->memory[++cpu->SP + STACK_START];
        break;
    case 'X':
        cpu->X = cpu->memory[++cpu->SP + STACK_START];
        break;
    case 'Y':
        cpu->Y = cpu->memory[++cpu->SP + STACK_START];
        break;
    case 'P':
        cpu->A = cpu->memory[++cpu->SP + STACK_START];
        CPU_onFlag(cpu, 'u'); // always on
        break;
    case 'C': // for PC
        addr = 0;
        low = cpu->memory[STACK_START + ++cpu->SP];
        high = cpu->memory[STACK_START + ++cpu->SP];
        addr = (low << 8) | high;
        cpu->PC = addr;
        break;
    default:
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "ERROR: non legal pop operation for register `%c`\n", reg);
        set_color(COLOR_RESET, stderr);
        break;
    }
}

bool CPU_onFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            cpu->P |= 0x80; // 0b10000000
            break;
        case 'v':
            cpu->P |= 0x40; // 0b01000000
            break;
        case 'u':
            cpu->P |= 0x20; // 0b00100000
            break;
        case 'b':
            cpu->P |= 0x10; // 0b00010000
            break;
        case 'd':
            cpu->P |= 0x08; // 0b00001000
            break;
        case 'i':
            cpu->P |= 0x04;  // 0b00000100
            break;
        case 'z':
            cpu->P |= 0x02; // 0b00000010
            break;
        case 'c':
            cpu->P |= 0x01; // 0b00000001
            break;
        default:
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            set_color(COLOR_RESET, stderr);
            return false;
    }
    return true;
}

bool CPU_offFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            cpu->P &= ~0x80; // 0b01111111
            break;
        case 'v':
            cpu->P &= ~0x40; // 0b10111111
            break;
        case 'b':
            cpu->P &= ~0x10; // 0b11101111
            break;
        case 'd':
            cpu->P &= ~0x08; // 0b11110111
            break;
        case 'i':
            cpu->P &= ~0x04;  // 0b11111011
            break;
        case 'z':
            cpu->P &= ~0x02; // 0b11111101
            break;
        case 'c':
            cpu->P &= ~0x01; // 0b11111110
            break;
        default:
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            set_color(COLOR_RESET, stderr);
            return false;
    }
    return true;
}

void CPU_tick(CPU* cpu, size_t amount){
    (void) cpu;
    (void) amount;
    return;
}

void CPU_invalid_opcode(CPU * cpu, byte opcode)
{
    // for debug
    set_color(COLOR_RED, stderr);
    fprintf(stderr, "ERROR: invalid opcode: `%s` code: $%.2x\n", opcode_to_cstr[opcode], opcode);
    set_color(COLOR_RESET, stderr);
    CPU_tick(cpu, 2);
}

void CPU_load_program_from_carr(CPU* cpu, word entry_point, byte* program, size_t program_size){
    word start = entry_point;
    for(size_t curr = 0; curr < program_size; curr++){
        cpu->memory[start + curr] = program[curr];
    }
}

// done at the hardware level not using normal cpu instructions.
// call wan the reset input is set low, not an interrupt!  
void CPU_reset(CPU *cpu)
{
    cpu->SP = RESET_SP_REGISTER;
    CPU_onFlag(cpu, 'i');
    CPU_offFlag(cpu, 'd');

    CPU_tick(cpu, 7);

    word bus;
    bus = cpu->memory[RESET_VECTOR_LOW_ADDER];
    bus += cpu->memory[RESET_VECTOR_HIGH_ADDER] * 0x0100;
    cpu->PC = bus;
}

void CPU_run(CPU* cpu, bool is_debug){
    while(true){

        if (is_debug) fprintf(stdout, "0x%.4x: 0x%.2x (%s)\n", 
            cpu->PC, 
            cpu->memory[cpu->PC], 
            opcode_to_cstr[cpu->memory[cpu->PC]] ? opcode_to_cstr[cpu->memory[cpu->PC]] : "???");

        // fetch:
        Opcode opcode = cpu->memory[cpu->PC++];

        // decode:
        Instruction instruction = Opcode_to_Instruction_table[opcode];
        if(!instruction){
            CPU_invalid_opcode(cpu, opcode);
            return;
        }
        
        // execute:
        instruction(cpu);
    }
}

void CPU_updateFlags(CPU* cpu, char reg, char flag, byte operand){
    byte value = 0;
    switch (reg)
    {
    case 'A':
        value = cpu->A;
        break;
    case 'X':
        value = cpu->X;
        break;
    case 'Y':
        value = cpu->Y;
        break;
    
    default:
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "ERROR: non legal register for appdate flags `%c`\n", reg);
        set_color(COLOR_RESET, stderr);
        return;
    }

    switch (flag)
    {
    case 'z':
        IS_ZERO(value) ? CPU_onFlag(cpu,'z') : CPU_offFlag(cpu,'z'); break;
    case 'v':
        IS_OVERFLOW(value, operand) ? CPU_onFlag(cpu,'v') : CPU_offFlag(cpu,'v'); break;
    case 'c':
        IS_CARRY(value, operand) ? CPU_onFlag(cpu,'c') : CPU_offFlag(cpu,'c'); break;
    case 'n':
        IS_NEGATIVE(value) ? CPU_onFlag(cpu,'n') : CPU_offFlag(cpu,'n'); break;
    default:
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "ERROR: non legal flag for appdate flags `%c`\n", flag);
        set_color(COLOR_RESET, stderr);
        break;
    }
}

