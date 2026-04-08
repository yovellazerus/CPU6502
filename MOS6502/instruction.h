#ifndef INSTRACTION_H_
#define INSTRACTION_H_

#include "CPU.h"

// Load/Store
void instruction_LDA_Immediate(CPU_t* CPU_t);
void instruction_LDA_ZeroPage(CPU_t* CPU_t);
void instruction_LDA_ZeroPageX(CPU_t* CPU_t);
void instruction_LDA_Absolute(CPU_t* CPU_t);
void instruction_LDA_AbsoluteX(CPU_t* CPU_t);
void instruction_LDA_AbsoluteY(CPU_t* CPU_t);
void instruction_LDA_IndirectX(CPU_t* CPU_t);
void instruction_LDA_IndirectY(CPU_t* CPU_t);

void instruction_LDX_Immediate(CPU_t* CPU_t);
void instruction_LDX_ZeroPage(CPU_t* CPU_t);
void instruction_LDX_ZeroPageY(CPU_t* CPU_t);
void instruction_LDX_Absolute(CPU_t* CPU_t);
void instruction_LDX_AbsoluteY(CPU_t* CPU_t);

void instruction_LDY_Immediate(CPU_t* CPU_t);
void instruction_LDY_ZeroPage(CPU_t* CPU_t);
void instruction_LDY_ZeroPageX(CPU_t* CPU_t);
void instruction_LDY_Absolute(CPU_t* CPU_t);
void instruction_LDY_AbsoluteX(CPU_t* CPU_t);

void instruction_STA_ZeroPage(CPU_t* CPU_t);
void instruction_STA_ZeroPageX(CPU_t* CPU_t);
void instruction_STA_Absolute(CPU_t* CPU_t);
void instruction_STA_AbsoluteX(CPU_t* CPU_t);
void instruction_STA_AbsoluteY(CPU_t* CPU_t);
void instruction_STA_IndirectX(CPU_t* CPU_t);
void instruction_STA_IndirectY(CPU_t* CPU_t);

void instruction_STX_ZeroPage(CPU_t* CPU_t);
void instruction_STX_ZeroPageY(CPU_t* CPU_t);
void instruction_STX_Absolute(CPU_t* CPU_t);

void instruction_STY_ZeroPage(CPU_t* CPU_t);
void instruction_STY_ZeroPageX(CPU_t* CPU_t);
void instruction_STY_Absolute(CPU_t* CPU_t);

// Register Transfers
void instruction_TAX(CPU_t* CPU_t);
void instruction_TAY(CPU_t* CPU_t);
void instruction_TSX(CPU_t* CPU_t);
void instruction_TXA(CPU_t* CPU_t);
void instruction_TXS(CPU_t* CPU_t);
void instruction_TYA(CPU_t* CPU_t);

// Stack
void instruction_PHA(CPU_t* CPU_t);
void instruction_PHP(CPU_t* CPU_t);
void instruction_PLA(CPU_t* CPU_t);
void instruction_PLP(CPU_t* CPU_t);

// Logical
void instruction_AND_Immediate(CPU_t* CPU_t);
void instruction_AND_ZeroPage(CPU_t* CPU_t);
void instruction_AND_ZeroPageX(CPU_t* CPU_t);
void instruction_AND_Absolute(CPU_t* CPU_t);
void instruction_AND_AbsoluteX(CPU_t* CPU_t);
void instruction_AND_AbsoluteY(CPU_t* CPU_t);
void instruction_AND_IndirectX(CPU_t* CPU_t);
void instruction_AND_IndirectY(CPU_t* CPU_t);

void instruction_EOR_Immediate(CPU_t* CPU_t);
void instruction_EOR_ZeroPage(CPU_t* CPU_t);
void instruction_EOR_ZeroPageX(CPU_t* CPU_t);
void instruction_EOR_Absolute(CPU_t* CPU_t);
void instruction_EOR_AbsoluteX(CPU_t* CPU_t);
void instruction_EOR_AbsoluteY(CPU_t* CPU_t);
void instruction_EOR_IndirectX(CPU_t* CPU_t);
void instruction_EOR_IndirectY(CPU_t* CPU_t);

void instruction_ORA_Immediate(CPU_t* CPU_t);
void instruction_ORA_ZeroPage(CPU_t* CPU_t);
void instruction_ORA_ZeroPageX(CPU_t* CPU_t);
void instruction_ORA_Absolute(CPU_t* CPU_t);
void instruction_ORA_AbsoluteX(CPU_t* CPU_t);
void instruction_ORA_AbsoluteY(CPU_t* CPU_t);
void instruction_ORA_IndirectX(CPU_t* CPU_t);
void instruction_ORA_IndirectY(CPU_t* CPU_t);

void instruction_BIT_ZeroPage(CPU_t* CPU_t);
void instruction_BIT_Absolute(CPU_t* CPU_t);

// Arithmetic
void instruction_ADC_Immediate(CPU_t* CPU_t);
void instruction_ADC_ZeroPage(CPU_t* CPU_t);
void instruction_ADC_ZeroPageX(CPU_t* CPU_t);
void instruction_ADC_Absolute(CPU_t* CPU_t);
void instruction_ADC_AbsoluteX(CPU_t* CPU_t);
void instruction_ADC_AbsoluteY(CPU_t* CPU_t);
void instruction_ADC_IndirectX(CPU_t* CPU_t);
void instruction_ADC_IndirectY(CPU_t* CPU_t);

void instruction_SBC_Immediate(CPU_t* CPU_t);
void instruction_SBC_ZeroPage(CPU_t* CPU_t);
void instruction_SBC_ZeroPageX(CPU_t* CPU_t);
void instruction_SBC_Absolute(CPU_t* CPU_t);
void instruction_SBC_AbsoluteX(CPU_t* CPU_t);
void instruction_SBC_AbsoluteY(CPU_t* CPU_t);
void instruction_SBC_IndirectX(CPU_t* CPU_t);
void instruction_SBC_IndirectY(CPU_t* CPU_t);

void instruction_CMP_Immediate(CPU_t* CPU_t);
void instruction_CMP_ZeroPage(CPU_t* CPU_t);
void instruction_CMP_ZeroPageX(CPU_t* CPU_t);
void instruction_CMP_Absolute(CPU_t* CPU_t);
void instruction_CMP_AbsoluteX(CPU_t* CPU_t);
void instruction_CMP_AbsoluteY(CPU_t* CPU_t);
void instruction_CMP_IndirectX(CPU_t* CPU_t);
void instruction_CMP_IndirectY(CPU_t* CPU_t);

void instruction_CPX_Immediate(CPU_t* CPU_t);
void instruction_CPX_ZeroPage(CPU_t* CPU_t);
void instruction_CPX_Absolute(CPU_t* CPU_t);

void instruction_CPY_Immediate(CPU_t* CPU_t);
void instruction_CPY_ZeroPage(CPU_t* CPU_t);
void instruction_CPY_Absolute(CPU_t* CPU_t);

// Increments / Decrements
void instruction_INC_ZeroPage(CPU_t* CPU_t);
void instruction_INC_ZeroPageX(CPU_t* CPU_t);
void instruction_INC_Absolute(CPU_t* CPU_t);
void instruction_INC_AbsoluteX(CPU_t* CPU_t);

void instruction_INX(CPU_t* CPU_t);
void instruction_INY(CPU_t* CPU_t);

void instruction_DEC_ZeroPage(CPU_t* CPU_t);
void instruction_DEC_ZeroPageX(CPU_t* CPU_t);
void instruction_DEC_Absolute(CPU_t* CPU_t);
void instruction_DEC_AbsoluteX(CPU_t* CPU_t);

void instruction_DEX(CPU_t* CPU_t);
void instruction_DEY(CPU_t* CPU_t);

// Shifts / Rotates
void instruction_ASL_Accumulator(CPU_t* CPU_t);
void instruction_ASL_ZeroPage(CPU_t* CPU_t);
void instruction_ASL_ZeroPageX(CPU_t* CPU_t);
void instruction_ASL_Absolute(CPU_t* CPU_t);
void instruction_ASL_AbsoluteX(CPU_t* CPU_t);

void instruction_LSR_Accumulator(CPU_t* CPU_t);
void instruction_LSR_ZeroPage(CPU_t* CPU_t);
void instruction_LSR_ZeroPageX(CPU_t* CPU_t);
void instruction_LSR_Absolute(CPU_t* CPU_t);
void instruction_LSR_AbsoluteX(CPU_t* CPU_t);

void instruction_ROL_Accumulator(CPU_t* CPU_t);
void instruction_ROL_ZeroPage(CPU_t* CPU_t);
void instruction_ROL_ZeroPageX(CPU_t* CPU_t);
void instruction_ROL_Absolute(CPU_t* CPU_t);
void instruction_ROL_AbsoluteX(CPU_t* CPU_t);

void instruction_ROR_Accumulator(CPU_t* CPU_t);
void instruction_ROR_ZeroPage(CPU_t* CPU_t);
void instruction_ROR_ZeroPageX(CPU_t* CPU_t);
void instruction_ROR_Absolute(CPU_t* CPU_t);
void instruction_ROR_AbsoluteX(CPU_t* CPU_t);

// Jumps / Calls
void instruction_JMP_Absolute(CPU_t* CPU_t);
void instruction_JMP_Indirect(CPU_t* CPU_t);
void instruction_JSR(CPU_t* CPU_t);
void instruction_RTS(CPU_t* CPU_t);

// Branches
void instruction_BCC(CPU_t* CPU_t);
void instruction_BCS(CPU_t* CPU_t);
void instruction_BEQ(CPU_t* CPU_t);
void instruction_BMI(CPU_t* CPU_t);
void instruction_BNE(CPU_t* CPU_t);
void instruction_BPL(CPU_t* CPU_t);
void instruction_BVC(CPU_t* CPU_t);
void instruction_BVS(CPU_t* CPU_t);

// Flags
void instruction_CLC(CPU_t* CPU_t);
void instruction_CLD(CPU_t* CPU_t);
void instruction_CLI(CPU_t* CPU_t);
void instruction_CLV(CPU_t* CPU_t);
void instruction_SEC(CPU_t* CPU_t);
void instruction_SED(CPU_t* CPU_t);
void instruction_SEI(CPU_t* CPU_t);

// System
void instruction_BRK(CPU_t* CPU_t);
void instruction_NOP(CPU_t* CPU_t);
void instruction_RTI(CPU_t* CPU_t);

#endif // INSTRACTION_H_
