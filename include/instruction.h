#ifndef INSTRACTION_H_
#define INSTRACTION_H_

#include "cpu.h"

typedef void (*Instruction)(CPU*);

extern Instruction Opcode_to_Instruction_table[0xff + 1];

// Load/Store
void instruction_LDA_Immediate(CPU* cpu);
void instruction_LDA_ZeroPage(CPU* cpu);
void instruction_LDA_ZeroPageX(CPU* cpu);
void instruction_LDA_Absolute(CPU* cpu);
void instruction_LDA_AbsoluteX(CPU* cpu);
void instruction_LDA_AbsoluteY(CPU* cpu);
void instruction_LDA_IndirectX(CPU* cpu);
void instruction_LDA_IndirectY(CPU* cpu);

void instruction_LDX_Immediate(CPU* cpu);
void instruction_LDX_ZeroPage(CPU* cpu);
void instruction_LDX_ZeroPageY(CPU* cpu);
void instruction_LDX_Absolute(CPU* cpu);
void instruction_LDX_AbsoluteY(CPU* cpu);

void instruction_LDY_Immediate(CPU* cpu);
void instruction_LDY_ZeroPage(CPU* cpu);
void instruction_LDY_ZeroPageX(CPU* cpu);
void instruction_LDY_Absolute(CPU* cpu);
void instruction_LDY_AbsoluteX(CPU* cpu);

void instruction_STA_ZeroPage(CPU* cpu);
void instruction_STA_ZeroPageX(CPU* cpu);
void instruction_STA_Absolute(CPU* cpu);
void instruction_STA_AbsoluteX(CPU* cpu);
void instruction_STA_AbsoluteY(CPU* cpu);
void instruction_STA_IndirectX(CPU* cpu);
void instruction_STA_IndirectY(CPU* cpu);

void instruction_STX_ZeroPage(CPU* cpu);
void instruction_STX_ZeroPageY(CPU* cpu);
void instruction_STX_Absolute(CPU* cpu);

void instruction_STY_ZeroPage(CPU* cpu);
void instruction_STY_ZeroPageX(CPU* cpu);
void instruction_STY_Absolute(CPU* cpu);

// Register Transfers
void instruction_TAX(CPU* cpu);
void instruction_TAY(CPU* cpu);
void instruction_TSX(CPU* cpu);
void instruction_TXA(CPU* cpu);
void instruction_TXS(CPU* cpu);
void instruction_TYA(CPU* cpu);

// Stack
void instruction_PHA(CPU* cpu);
void instruction_PHP(CPU* cpu);
void instruction_PLA(CPU* cpu);
void instruction_PLP(CPU* cpu);

// Logical
void instruction_AND_Immediate(CPU* cpu);
void instruction_AND_ZeroPage(CPU* cpu);
void instruction_AND_ZeroPageX(CPU* cpu);
void instruction_AND_Absolute(CPU* cpu);
void instruction_AND_AbsoluteX(CPU* cpu);
void instruction_AND_AbsoluteY(CPU* cpu);
void instruction_AND_IndirectX(CPU* cpu);
void instruction_AND_IndirectY(CPU* cpu);

void instruction_EOR_Immediate(CPU* cpu);
void instruction_EOR_ZeroPage(CPU* cpu);
void instruction_EOR_ZeroPageX(CPU* cpu);
void instruction_EOR_Absolute(CPU* cpu);
void instruction_EOR_AbsoluteX(CPU* cpu);
void instruction_EOR_AbsoluteY(CPU* cpu);
void instruction_EOR_IndirectX(CPU* cpu);
void instruction_EOR_IndirectY(CPU* cpu);

void instruction_ORA_Immediate(CPU* cpu);
void instruction_ORA_ZeroPage(CPU* cpu);
void instruction_ORA_ZeroPageX(CPU* cpu);
void instruction_ORA_Absolute(CPU* cpu);
void instruction_ORA_AbsoluteX(CPU* cpu);
void instruction_ORA_AbsoluteY(CPU* cpu);
void instruction_ORA_IndirectX(CPU* cpu);
void instruction_ORA_IndirectY(CPU* cpu);

void instruction_BIT_ZeroPage(CPU* cpu);
void instruction_BIT_Absolute(CPU* cpu);

// Arithmetic
void instruction_ADC_Immediate(CPU* cpu);
void instruction_ADC_ZeroPage(CPU* cpu);
void instruction_ADC_ZeroPageX(CPU* cpu);
void instruction_ADC_Absolute(CPU* cpu);
void instruction_ADC_AbsoluteX(CPU* cpu);
void instruction_ADC_AbsoluteY(CPU* cpu);
void instruction_ADC_IndirectX(CPU* cpu);
void instruction_ADC_IndirectY(CPU* cpu);

void instruction_SBC_Immediate(CPU* cpu);
void instruction_SBC_ZeroPage(CPU* cpu);
void instruction_SBC_ZeroPageX(CPU* cpu);
void instruction_SBC_Absolute(CPU* cpu);
void instruction_SBC_AbsoluteX(CPU* cpu);
void instruction_SBC_AbsoluteY(CPU* cpu);
void instruction_SBC_IndirectX(CPU* cpu);
void instruction_SBC_IndirectY(CPU* cpu);

void instruction_CMP_Immediate(CPU* cpu);
void instruction_CMP_ZeroPage(CPU* cpu);
void instruction_CMP_ZeroPageX(CPU* cpu);
void instruction_CMP_Absolute(CPU* cpu);
void instruction_CMP_AbsoluteX(CPU* cpu);
void instruction_CMP_AbsoluteY(CPU* cpu);
void instruction_CMP_IndirectX(CPU* cpu);
void instruction_CMP_IndirectY(CPU* cpu);

void instruction_CPX_Immediate(CPU* cpu);
void instruction_CPX_ZeroPage(CPU* cpu);
void instruction_CPX_Absolute(CPU* cpu);

void instruction_CPY_Immediate(CPU* cpu);
void instruction_CPY_ZeroPage(CPU* cpu);
void instruction_CPY_Absolute(CPU* cpu);

// Increments / Decrements
void instruction_INC_ZeroPage(CPU* cpu);
void instruction_INC_ZeroPageX(CPU* cpu);
void instruction_INC_Absolute(CPU* cpu);
void instruction_INC_AbsoluteX(CPU* cpu);

void instruction_INX(CPU* cpu);
void instruction_INY(CPU* cpu);

void instruction_DEC_ZeroPage(CPU* cpu);
void instruction_DEC_ZeroPageX(CPU* cpu);
void instruction_DEC_Absolute(CPU* cpu);
void instruction_DEC_AbsoluteX(CPU* cpu);

void instruction_DEX(CPU* cpu);
void instruction_DEY(CPU* cpu);

// Shifts / Rotates
void instruction_ASL_Accumulator(CPU* cpu);
void instruction_ASL_ZeroPage(CPU* cpu);
void instruction_ASL_ZeroPageX(CPU* cpu);
void instruction_ASL_Absolute(CPU* cpu);
void instruction_ASL_AbsoluteX(CPU* cpu);

void instruction_LSR_Accumulator(CPU* cpu);
void instruction_LSR_ZeroPage(CPU* cpu);
void instruction_LSR_ZeroPageX(CPU* cpu);
void instruction_LSR_Absolute(CPU* cpu);
void instruction_LSR_AbsoluteX(CPU* cpu);

void instruction_ROL_Accumulator(CPU* cpu);
void instruction_ROL_ZeroPage(CPU* cpu);
void instruction_ROL_ZeroPageX(CPU* cpu);
void instruction_ROL_Absolute(CPU* cpu);
void instruction_ROL_AbsoluteX(CPU* cpu);

void instruction_ROR_Accumulator(CPU* cpu);
void instruction_ROR_ZeroPage(CPU* cpu);
void instruction_ROR_ZeroPageX(CPU* cpu);
void instruction_ROR_Absolute(CPU* cpu);
void instruction_ROR_AbsoluteX(CPU* cpu);

// Jumps / Calls
void instruction_JMP_Absolute(CPU* cpu);
void instruction_JMP_Indirect(CPU* cpu);
void instruction_JSR(CPU* cpu);
void instruction_RTS(CPU* cpu);

// Branches
void instruction_BCC(CPU* cpu);
void instruction_BCS(CPU* cpu);
void instruction_BEQ(CPU* cpu);
void instruction_BMI(CPU* cpu);
void instruction_BNE(CPU* cpu);
void instruction_BPL(CPU* cpu);
void instruction_BVC(CPU* cpu);
void instruction_BVS(CPU* cpu);

// Flags
void instruction_CLC(CPU* cpu);
void instruction_CLD(CPU* cpu);
void instruction_CLI(CPU* cpu);
void instruction_CLV(CPU* cpu);
void instruction_SEC(CPU* cpu);
void instruction_SED(CPU* cpu);
void instruction_SEI(CPU* cpu);

// System
void instruction_BRK(CPU* cpu);
void instruction_NOP(CPU* cpu);
void instruction_RTI(CPU* cpu);

#endif // INSTRACTION_H_
