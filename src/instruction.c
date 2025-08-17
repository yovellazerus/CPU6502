
#include "../include/instruction.h"

Instruction Opcode_to_Instraction_table[0xff + 1] = {
    // Load/Store
    [Opcode_LDA_Immediate]   = instruction_LDA_Immediate,
    [Opcode_LDA_ZeroPage]    = instruction_LDA_ZeroPage,
    [Opcode_LDA_ZeroPageX]   = instruction_LDA_ZeroPageX,
    [Opcode_LDA_Absolute]    = instruction_LDA_Absolute,
    [Opcode_LDA_AbsoluteX]   = instruction_LDA_AbsoluteX,
    [Opcode_LDA_AbsoluteY]   = instruction_LDA_AbsoluteY,
    [Opcode_LDA_IndirectX]   = instruction_LDA_IndirectX,
    [Opcode_LDA_IndirectY]   = instruction_LDA_IndirectY,

    [Opcode_LDX_Immediate]   = instruction_LDX_Immediate,
    [Opcode_LDX_ZeroPage]    = instruction_LDX_ZeroPage,
    [Opcode_LDX_ZeroPageY]   = instruction_LDX_ZeroPageY,
    [Opcode_LDX_Absolute]    = instruction_LDX_Absolute,
    [Opcode_LDX_AbsoluteY]   = instruction_LDX_AbsoluteY,

    [Opcode_LDY_Immediate]   = instruction_LDY_Immediate,
    [Opcode_LDY_ZeroPage]    = instruction_LDY_ZeroPage,
    [Opcode_LDY_ZeroPageX]   = instruction_LDY_ZeroPageX,
    [Opcode_LDY_Absolute]    = instruction_LDY_Absolute,
    [Opcode_LDY_AbsoluteX]   = instruction_LDY_AbsoluteX,

    [Opcode_STA_ZeroPage]    = instruction_STA_ZeroPage,
    [Opcode_STA_ZeroPageX]   = instruction_STA_ZeroPageX,
    [Opcode_STA_Absolute]    = instruction_STA_Absolute,
    [Opcode_STA_AbsoluteX]   = instruction_STA_AbsoluteX,
    [Opcode_STA_AbsoluteY]   = instruction_STA_AbsoluteY,
    [Opcode_STA_IndirectX]   = instruction_STA_IndirectX,
    [Opcode_STA_IndirectY]   = instruction_STA_IndirectY,

    [Opcode_STX_ZeroPage]    = instruction_STX_ZeroPage,
    [Opcode_STX_ZeroPageY]   = instruction_STX_ZeroPageY,
    [Opcode_STX_Absolute]    = instruction_STX_Absolute,

    [Opcode_STY_ZeroPage]    = instruction_STY_ZeroPage,
    [Opcode_STY_ZeroPageX]   = instruction_STY_ZeroPageX,
    [Opcode_STY_Absolute]    = instruction_STY_Absolute,

    // Register Transfers
    [Opcode_TAX]             = instruction_TAX,
    [Opcode_TAY]             = instruction_TAY,
    [Opcode_TSX]             = instruction_TSX,
    [Opcode_TXA]             = instruction_TXA,
    [Opcode_TXS]             = instruction_TXS,
    [Opcode_TYA]             = instruction_TYA,

    // Stack
    [Opcode_PHA]             = instruction_PHA,
    [Opcode_PHP]             = instruction_PHP,
    [Opcode_PLA]             = instruction_PLA,
    [Opcode_PLP]             = instruction_PLP,

    // Logical
    [Opcode_AND_Immediate]   = instruction_AND_Immediate,
    [Opcode_AND_ZeroPage]    = instruction_AND_ZeroPage,
    [Opcode_AND_ZeroPageX]   = instruction_AND_ZeroPageX,
    [Opcode_AND_Absolute]    = instruction_AND_Absolute,
    [Opcode_AND_AbsoluteX]   = instruction_AND_AbsoluteX,
    [Opcode_AND_AbsoluteY]   = instruction_AND_AbsoluteY,
    [Opcode_AND_IndirectX]   = instruction_AND_IndirectX,
    [Opcode_AND_IndirectY]   = instruction_AND_IndirectY,

    [Opcode_EOR_Immediate]   = instruction_EOR_Immediate,
    [Opcode_EOR_ZeroPage]    = instruction_EOR_ZeroPage,
    [Opcode_EOR_ZeroPageX]   = instruction_EOR_ZeroPageX,
    [Opcode_EOR_Absolute]    = instruction_EOR_Absolute,
    [Opcode_EOR_AbsoluteX]   = instruction_EOR_AbsoluteX,
    [Opcode_EOR_AbsoluteY]   = instruction_EOR_AbsoluteY,
    [Opcode_EOR_IndirectX]   = instruction_EOR_IndirectX,
    [Opcode_EOR_IndirectY]   = instruction_EOR_IndirectY,

    [Opcode_ORA_Immediate]   = instruction_ORA_Immediate,
    [Opcode_ORA_ZeroPage]    = instruction_ORA_ZeroPage,
    [Opcode_ORA_ZeroPageX]   = instruction_ORA_ZeroPageX,
    [Opcode_ORA_Absolute]    = instruction_ORA_Absolute,
    [Opcode_ORA_AbsoluteX]   = instruction_ORA_AbsoluteX,
    [Opcode_ORA_AbsoluteY]   = instruction_ORA_AbsoluteY,
    [Opcode_ORA_IndirectX]   = instruction_ORA_IndirectX,
    [Opcode_ORA_IndirectY]   = instruction_ORA_IndirectY,

    [Opcode_BIT_ZeroPage]    = instruction_BIT_ZeroPage,
    [Opcode_BIT_Absolute]    = instruction_BIT_Absolute,

    // Arithmetic
    [Opcode_ADC_Immediate]   = instruction_ADC_Immediate,
    [Opcode_ADC_ZeroPage]    = instruction_ADC_ZeroPage,
    [Opcode_ADC_ZeroPageX]   = instruction_ADC_ZeroPageX,
    [Opcode_ADC_Absolute]    = instruction_ADC_Absolute,
    [Opcode_ADC_AbsoluteX]   = instruction_ADC_AbsoluteX,
    [Opcode_ADC_AbsoluteY]   = instruction_ADC_AbsoluteY,
    [Opcode_ADC_IndirectX]   = instruction_ADC_IndirectX,
    [Opcode_ADC_IndirectY]   = instruction_ADC_IndirectY,

    [Opcode_SBC_Immediate]   = instruction_SBC_Immediate,
    [Opcode_SBC_ZeroPage]    = instruction_SBC_ZeroPage,
    [Opcode_SBC_ZeroPageX]   = instruction_SBC_ZeroPageX,
    [Opcode_SBC_Absolute]    = instruction_SBC_Absolute,
    [Opcode_SBC_AbsoluteX]   = instruction_SBC_AbsoluteX,
    [Opcode_SBC_AbsoluteY]   = instruction_SBC_AbsoluteY,
    [Opcode_SBC_IndirectX]   = instruction_SBC_IndirectX,
    [Opcode_SBC_IndirectY]   = instruction_SBC_IndirectY,

    [Opcode_CMP_Immediate]   = instruction_CMP_Immediate,
    [Opcode_CMP_ZeroPage]    = instruction_CMP_ZeroPage,
    [Opcode_CMP_ZeroPageX]   = instruction_CMP_ZeroPageX,
    [Opcode_CMP_Absolute]    = instruction_CMP_Absolute,
    [Opcode_CMP_AbsoluteX]   = instruction_CMP_AbsoluteX,
    [Opcode_CMP_AbsoluteY]   = instruction_CMP_AbsoluteY,
    [Opcode_CMP_IndirectX]   = instruction_CMP_IndirectX,
    [Opcode_CMP_IndirectY]   = instruction_CMP_IndirectY,

    [Opcode_CPX_Immediate]   = instruction_CPX_Immediate,
    [Opcode_CPX_ZeroPage]    = instruction_CPX_ZeroPage,
    [Opcode_CPX_Absolute]    = instruction_CPX_Absolute,

    [Opcode_CPY_Immediate]   = instruction_CPY_Immediate,
    [Opcode_CPY_ZeroPage]    = instruction_CPY_ZeroPage,
    [Opcode_CPY_Absolute]    = instruction_CPY_Absolute,

    // Increments / Decrements
    [Opcode_INC_ZeroPage]    = instruction_INC_ZeroPage,
    [Opcode_INC_ZeroPageX]   = instruction_INC_ZeroPageX,
    [Opcode_INC_Absolute]    = instruction_INC_Absolute,
    [Opcode_INC_AbsoluteX]   = instruction_INC_AbsoluteX,

    [Opcode_INX]             = instruction_INX,
    [Opcode_INY]             = instruction_INY,

    [Opcode_DEC_ZeroPage]    = instruction_DEC_ZeroPage,
    [Opcode_DEC_ZeroPageX]   = instruction_DEC_ZeroPageX,
    [Opcode_DEC_Absolute]    = instruction_DEC_Absolute,
    [Opcode_DEC_AbsoluteX]   = instruction_DEC_AbsoluteX,

    [Opcode_DEX]             = instruction_DEX,
    [Opcode_DEY]             = instruction_DEY,

    // Shifts / Rotates
    [Opcode_ASL_Accumulator] = instruction_ASL_Accumulator,
    [Opcode_ASL_ZeroPage]    = instruction_ASL_ZeroPage,
    [Opcode_ASL_ZeroPageX]   = instruction_ASL_ZeroPageX,
    [Opcode_ASL_Absolute]    = instruction_ASL_Absolute,
    [Opcode_ASL_AbsoluteX]   = instruction_ASL_AbsoluteX,

    [Opcode_LSR_Accumulator] = instruction_LSR_Accumulator,
    [Opcode_LSR_ZeroPage]    = instruction_LSR_ZeroPage,
    [Opcode_LSR_ZeroPageX]   = instruction_LSR_ZeroPageX,
    [Opcode_LSR_Absolute]    = instruction_LSR_Absolute,
    [Opcode_LSR_AbsoluteX]   = instruction_LSR_AbsoluteX,

    [Opcode_ROL_Accumulator] = instruction_ROL_Accumulator,
    [Opcode_ROL_ZeroPage]    = instruction_ROL_ZeroPage,
    [Opcode_ROL_ZeroPageX]   = instruction_ROL_ZeroPageX,
    [Opcode_ROL_Absolute]    = instruction_ROL_Absolute,
    [Opcode_ROL_AbsoluteX]   = instruction_ROL_AbsoluteX,

    [Opcode_ROR_Accumulator] = instruction_ROR_Accumulator,
    [Opcode_ROR_ZeroPage]    = instruction_ROR_ZeroPage,
    [Opcode_ROR_ZeroPageX]   = instruction_ROR_ZeroPageX,
    [Opcode_ROR_Absolute]    = instruction_ROR_Absolute,
    [Opcode_ROR_AbsoluteX]   = instruction_ROR_AbsoluteX,

    // Jumps & Calls
    [Opcode_JMP_Absolute]    = instruction_JMP_Absolute,
    [Opcode_JMP_Indirect]    = instruction_JMP_Indirect,
    [Opcode_JSR]             = instruction_JSR,
    [Opcode_RTS]             = instruction_RTS,

    // Branches
    [Opcode_BCC]             = instruction_BCC,
    [Opcode_BCS]             = instruction_BCS,
    [Opcode_BEQ]             = instruction_BEQ,
    [Opcode_BMI]             = instruction_BMI,
    [Opcode_BNE]             = instruction_BNE,
    [Opcode_BPL]             = instruction_BPL,
    [Opcode_BVC]             = instruction_BVC,
    [Opcode_BVS]             = instruction_BVS,

    // Flags
    [Opcode_CLC]             = instruction_CLC,
    [Opcode_CLD]             = instruction_CLD,
    [Opcode_CLI]             = instruction_CLI,
    [Opcode_CLV]             = instruction_CLV,
    [Opcode_SEC]             = instruction_SEC,
    [Opcode_SED]             = instruction_SED,
    [Opcode_SEI]             = instruction_SEI,

    // System
    [Opcode_BRK]             = instruction_BRK,
    [Opcode_NOP]             = instruction_NOP,
    [Opcode_RTI]             = instruction_RTI,

    // debug
    [Opcode_HLT_debug]       = instruction_HLT_debug,
};

void instruction_LDA_Immediate(CPU* cpu) 
{
    byte immediate = cpu->memory[cpu->PC++];
    cpu->A = immediate;
    if(immediate == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(immediate < 0){
        CPU_onFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2); 
}
void instruction_LDA_ZeroPage(CPU* cpu) 
{

}
void instruction_LDA_ZeroPageX(CPU* cpu) 
{

}
void instruction_LDA_Absolute(CPU* cpu) 
{

}
void instruction_LDA_AbsoluteX(CPU* cpu) 
{

}
void instruction_LDA_AbsoluteY(CPU* cpu) 
{

}
void instruction_LDA_IndirectX(CPU* cpu) 
{

}
void instruction_LDA_IndirectY(CPU* cpu) 
{

}

void instruction_LDX_Immediate(CPU* cpu)
{

}
void instruction_LDX_ZeroPage(CPU* cpu)
{

}
void instruction_LDX_ZeroPageY(CPU* cpu)
{

}
void instruction_LDX_Absolute(CPU* cpu)
{

}
void instruction_LDX_AbsoluteY(CPU* cpu)
{

}

void instruction_LDY_Immediate(CPU* cpu)
{

}
void instruction_LDY_ZeroPage(CPU* cpu)
{

}
void instruction_LDY_ZeroPageX(CPU* cpu)
{

}
void instruction_LDY_Absolute(CPU* cpu)
{

}
void instruction_LDY_AbsoluteX(CPU* cpu)
{

}

void instruction_STA_ZeroPage(CPU* cpu)
{

}
void instruction_STA_ZeroPageX(CPU* cpu) 
{

}
void instruction_STA_Absolute(CPU* cpu)
{

}
void instruction_STA_AbsoluteX(CPU* cpu)
{

}
void instruction_STA_AbsoluteY(CPU* cpu)
{

}
void instruction_STA_IndirectX(CPU* cpu)
{

}
void instruction_STA_IndirectY(CPU* cpu)
{

}

void instruction_STX_ZeroPage(CPU* cpu)
{

}
void instruction_STX_ZeroPageY(CPU* cpu)
{

}
void instruction_STX_Absolute(CPU* cpu)
{

}

void instruction_STY_ZeroPage(CPU* cpu)
{

}
void instruction_STY_ZeroPageX(CPU* cpu)
{

}
void instruction_STY_Absolute(CPU* cpu)
{

}

// Register Transfers
void instruction_TAX(CPU* cpu)
{

}
void instruction_TAY(CPU* cpu)
{

}
void instruction_TSX(CPU* cpu)
{

}
void instruction_TXA(CPU* cpu)
{

}
void instruction_TXS(CPU* cpu)
{

}
void instruction_TYA(CPU* cpu)
{

}

// Stack
void instruction_PHA(CPU* cpu)
{

}
void instruction_PHP(CPU* cpu)
{

}
void instruction_PLA(CPU* cpu)
{

}
void instruction_PLP(CPU* cpu)
{

}

// Logical
void instruction_AND_Immediate(CPU* cpu)
{

}
void instruction_AND_ZeroPage(CPU* cpu)
{

}
void instruction_AND_ZeroPageX(CPU* cpu)
{

}
void instruction_AND_Absolute(CPU* cpu)
{

}
void instruction_AND_AbsoluteX(CPU* cpu)
{

}
void instruction_AND_AbsoluteY(CPU* cpu)
{

}
void instruction_AND_IndirectX(CPU* cpu)
{

}
void instruction_AND_IndirectY(CPU* cpu)
{

}

void instruction_EOR_Immediate(CPU* cpu)
{

}
void instruction_EOR_ZeroPage(CPU* cpu)
{

}
void instruction_EOR_ZeroPageX(CPU* cpu)
{

}
void instruction_EOR_Absolute(CPU* cpu)
{

}
void instruction_EOR_AbsoluteX(CPU* cpu)
{

}
void instruction_EOR_AbsoluteY(CPU* cpu)
{

}
void instruction_EOR_IndirectX(CPU* cpu)
{

}
void instruction_EOR_IndirectY(CPU* cpu)
{

}

void instruction_ORA_Immediate(CPU* cpu)
{

}
void instruction_ORA_ZeroPage(CPU* cpu)
{

}
void instruction_ORA_ZeroPageX(CPU* cpu)
{

}
void instruction_ORA_Absolute(CPU* cpu)
{

}
void instruction_ORA_AbsoluteX(CPU* cpu)
{

}
void instruction_ORA_AbsoluteY(CPU* cpu)
{

}
void instruction_ORA_IndirectX(CPU* cpu)
{

}
void instruction_ORA_IndirectY(CPU* cpu)
{

}

void instruction_BIT_ZeroPage(CPU* cpu)
{

}
void instruction_BIT_Absolute(CPU* cpu)
{

}

// Arithmetic
void instruction_ADC_Immediate(CPU* cpu)
{

}
void instruction_ADC_ZeroPage(CPU* cpu)
{

}
void instruction_ADC_ZeroPageX(CPU* cpu)
{

}
void instruction_ADC_Absolute(CPU* cpu)
{

}
void instruction_ADC_AbsoluteX(CPU* cpu)
{

}
void instruction_ADC_AbsoluteY(CPU* cpu)
{

}
void instruction_ADC_IndirectX(CPU* cpu)
{

}
void instruction_ADC_IndirectY(CPU* cpu)
{

}

void instruction_SBC_Immediate(CPU* cpu)
{

}
void instruction_SBC_ZeroPage(CPU* cpu)
{

}
void instruction_SBC_ZeroPageX(CPU* cpu)
{

}
void instruction_SBC_Absolute(CPU* cpu)
{

}
void instruction_SBC_AbsoluteX(CPU* cpu)
{

}
void instruction_SBC_AbsoluteY(CPU* cpu)
{

}
void instruction_SBC_IndirectX(CPU* cpu)
{

}
void instruction_SBC_IndirectY(CPU* cpu)
{

}

void instruction_CMP_Immediate(CPU* cpu){

}
void instruction_CMP_ZeroPage(CPU* cpu){

}
void instruction_CMP_ZeroPageX(CPU* cpu){

}
void instruction_CMP_Absolute(CPU* cpu){

}
void instruction_CMP_AbsoluteX(CPU* cpu){

}
void instruction_CMP_AbsoluteY(CPU* cpu){

}
void instruction_CMP_IndirectX(CPU* cpu){

}
void instruction_CMP_IndirectY(CPU* cpu){

}

void instruction_CPX_Immediate(CPU* cpu)
{

}
void instruction_CPX_ZeroPage(CPU* cpu)
{

}
void instruction_CPX_Absolute(CPU* cpu)
{

}
void instruction_CPY_Immediate(CPU* cpu)
{

}
void instruction_CPY_ZeroPage(CPU* cpu)
{

}
void instruction_CPY_Absolute(CPU* cpu)
{

}

// Increments / Decrements
void instruction_INC_ZeroPage(CPU* cpu)
{

}
void instruction_INC_ZeroPageX(CPU* cpu)
{

}
void instruction_INC_Absolute(CPU* cpu)
{

}
void instruction_INC_AbsoluteX(CPU* cpu)
{

}

void instruction_INX(CPU* cpu){

}
void instruction_INY(CPU* cpu){

}

void instruction_DEC_ZeroPage(CPU* cpu)
{

}
void instruction_DEC_ZeroPageX(CPU* cpu)
{

}
void instruction_DEC_Absolute(CPU* cpu)
{

}
void instruction_DEC_AbsoluteX(CPU* cpu)
{

}

void instruction_DEX(CPU* cpu)
{

}
void instruction_DEY(CPU* cpu)
{

}

// Shifts / Rotates
void instruction_ASL_Accumulator(CPU* cpu)
{

}
void instruction_ASL_ZeroPage(CPU* cpu)
{

}
void instruction_ASL_ZeroPageX(CPU* cpu)
{

}
void instruction_ASL_Absolute(CPU* cpu)
{

}
void instruction_ASL_AbsoluteX(CPU* cpu)
{

}

void instruction_LSR_Accumulator(CPU* cpu)
{

}
void instruction_LSR_ZeroPage(CPU* cpu)
{

}
void instruction_LSR_ZeroPageX(CPU* cpu)
{

}
void instruction_LSR_Absolute(CPU* cpu)
{

}
void instruction_LSR_AbsoluteX(CPU* cpu)
{

}

void instruction_ROL_Accumulator(CPU* cpu)
{

}
void instruction_ROL_ZeroPage(CPU* cpu)
{

}
void instruction_ROL_ZeroPageX(CPU* cpu)
{

}
void instruction_ROL_Absolute(CPU* cpu)
{

}
void instruction_ROL_AbsoluteX(CPU* cpu)
{

}

void instruction_ROR_Accumulator(CPU* cpu)
{

}
void instruction_ROR_ZeroPage(CPU* cpu)
{

}
void instruction_ROR_ZeroPageX(CPU* cpu)
{

}
void instruction_ROR_Absolute(CPU* cpu)
{

}
void instruction_ROR_AbsoluteX(CPU* cpu)
{

}

// Jumps / Calls
void instruction_JMP_Absolute(CPU* cpu)
{

}
void instruction_JMP_Indirect(CPU* cpu)
{

}
void instruction_JSR(CPU* cpu)
{

}
void instruction_RTS(CPU* cpu)
{

}

// Branches
void instruction_BCC(CPU* cpu)
{

}
void instruction_BCS(CPU* cpu)
{

}
void instruction_BEQ(CPU* cpu)
{

}
void instruction_BMI(CPU* cpu)
{

}
void instruction_BNE(CPU* cpu)
{

}
void instruction_BPL(CPU* cpu)
{

}
void instruction_BVC(CPU* cpu)
{

}
void instruction_BVS(CPU* cpu)
{

}

// Flags
void instruction_CLC(CPU* cpu)
{

}
void instruction_CLD(CPU* cpu)
{

}
void instruction_CLI(CPU* cpu)
{

}
void instruction_CLV(CPU* cpu)
{

}
void instruction_SEC(CPU* cpu)
{

}
void instruction_SED(CPU* cpu)
{

}
void instruction_SEI(CPU* cpu)
{

}

// System
void instruction_BRK(CPU* cpu){
    
}
void instruction_NOP(CPU* cpu){
    
}
void instruction_RTI(CPU* cpu){
}

void instruction_HLT_debug(CPU *cpu)
{
    cpu->halt = true;
}