
#include "../include\cpu.h"

const char* Addressing_mode_to_cstr[count_Add + 1] = {
    [Add_non] = "Add_non",
    [Add_Brk] = "Add_Brk",
    [Add_Relative] = "Add_Relative",
    [Add_Implied] = "Add_Implied",
    [Add_Immediate] = "Add_Immediate",
    [Add_Accumulator] = "Add_Accumulator",
    [Add_Absolute] = "Add_Absolute",
    [Add_AbsoluteX] = "Add_AbsoluteX",
    [Add_AbsoluteY] = "Add_AbsoluteY",
    [Add_ZeroPage] = "Add_ZeroPage",
    [Add_ZeroPageX] = "Add_ZeroPageX",
    [Add_ZeroPageY] = "Add_ZeroPageY",
    [Add_Indirect] = "Add_Indirect",
    [Add_IndirectX] = "Add_IndirectX",
    [Add_IndirectY] = "Add_IndirectY",

    [count_Add] = "Add_UNKNOWN",
};

const char* opcode_to_cstr[0xff + 1] = {

    [Opcode_LDA_Immediate]   = "LDA",
    [Opcode_LDA_ZeroPage ]   = "LDA",
    [Opcode_LDA_ZeroPageX]   = "LDA",
    [Opcode_LDA_Absolute ]   = "LDA",
    [Opcode_LDA_AbsoluteX]   = "LDA",
    [Opcode_LDA_AbsoluteY]   = "LDA",
    [Opcode_LDA_IndirectX]   = "LDA",
    [Opcode_LDA_IndirectY]   = "LDA",
    [Opcode_LDX_Immediate]   = "LDX",
    [Opcode_LDX_ZeroPage ]   = "LDX",
    [Opcode_LDX_ZeroPageY]   = "LDX",
    [Opcode_LDX_Absolute ]   = "LDX",
    [Opcode_LDX_AbsoluteY]   = "LDX",
    [Opcode_LDY_Immediate]   = "LDY",
    [Opcode_LDY_ZeroPage ]   = "LDY",
    [Opcode_LDY_ZeroPageX]   = "LDY",
    [Opcode_LDY_Absolute ]   = "LDY",
    [Opcode_LDY_AbsoluteX]   = "LDY",
    [Opcode_STA_ZeroPage ]   = "STA",
    [Opcode_STA_ZeroPageX]   = "STA",
    [Opcode_STA_Absolute ]   = "STA",
    [Opcode_STA_AbsoluteX]   = "STA",
    [Opcode_STA_AbsoluteY]   = "STA",
    [Opcode_STA_IndirectX]   = "STA",
    [Opcode_STA_IndirectY]   = "STA",
    [Opcode_STX_ZeroPage ]   = "STX",
    [Opcode_STX_ZeroPageY]   = "STX",
    [Opcode_STX_Absolute ]   = "STX",
    [Opcode_STY_ZeroPage ]   = "STY",
    [Opcode_STY_ZeroPageX]   = "STY",
    [Opcode_STY_Absolute ]   = "STY",
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
    [Opcode_AND_Immediate]   = "AND",
    [Opcode_AND_ZeroPage ]   = "AND",
    [Opcode_AND_ZeroPageX]   = "AND",
    [Opcode_AND_Absolute ]   = "AND",
    [Opcode_AND_AbsoluteX]   = "AND",
    [Opcode_AND_AbsoluteY]   = "AND",
    [Opcode_AND_IndirectX]   = "AND",
    [Opcode_AND_IndirectY]   = "AND",
    [Opcode_EOR_Immediate]   = "EOR",
    [Opcode_EOR_ZeroPage ]   = "EOR",
    [Opcode_EOR_ZeroPageX]   = "EOR",
    [Opcode_EOR_Absolute ]   = "EOR",
    [Opcode_EOR_AbsoluteX]   = "EOR",
    [Opcode_EOR_AbsoluteY]   = "EOR",
    [Opcode_EOR_IndirectX]   = "EOR",
    [Opcode_EOR_IndirectY]   = "EOR",
    [Opcode_ORA_Immediate]   = "ORA",
    [Opcode_ORA_ZeroPage ]   = "ORA",
    [Opcode_ORA_ZeroPageX]   = "ORA",
    [Opcode_ORA_Absolute ]   = "ORA",
    [Opcode_ORA_AbsoluteX]   = "ORA",
    [Opcode_ORA_AbsoluteY]   = "ORA",
    [Opcode_ORA_IndirectX]   = "ORA",
    [Opcode_ORA_IndirectY]   = "ORA",
    [Opcode_BIT_ZeroPage ]   = "BIT",
    [Opcode_BIT_Absolute ]   = "BIT",
    [Opcode_ADC_Immediate]   = "ADC",
    [Opcode_ADC_ZeroPage ]   = "ADC",
    [Opcode_ADC_ZeroPageX]   = "ADC",
    [Opcode_ADC_Absolute ]   = "ADC",
    [Opcode_ADC_AbsoluteX]   = "ADC",
    [Opcode_ADC_AbsoluteY]   = "ADC",
    [Opcode_ADC_IndirectX]   = "ADC",
    [Opcode_ADC_IndirectY]   = "ADC",
    [Opcode_SBC_Immediate]   = "SBC",
    [Opcode_SBC_ZeroPage ]   = "SBC",
    [Opcode_SBC_ZeroPageX]   = "SBC",
    [Opcode_SBC_Absolute ]   = "SBC",
    [Opcode_SBC_AbsoluteX]   = "SBC",
    [Opcode_SBC_AbsoluteY]   = "SBC",
    [Opcode_SBC_IndirectX]   = "SBC",
    [Opcode_SBC_IndirectY]   = "SBC",
    [Opcode_CMP_Immediate]   = "CMP",
    [Opcode_CMP_ZeroPage ]   = "CMP",
    [Opcode_CMP_ZeroPageX]   = "CMP",
    [Opcode_CMP_Absolute ]   = "CMP",
    [Opcode_CMP_AbsoluteX]   = "CMP",
    [Opcode_CMP_AbsoluteY]   = "CMP",
    [Opcode_CMP_IndirectX]   = "CMP",
    [Opcode_CMP_IndirectY]   = "CMP",
    [Opcode_CPX_Immediate]   = "CPX",
    [Opcode_CPX_ZeroPage ]   = "CPX",
    [Opcode_CPX_Absolute ]   = "CPX",
    [Opcode_CPY_Immediate]   = "CPY",
    [Opcode_CPY_ZeroPage ]   = "CPY",
    [Opcode_CPY_Absolute ]   = "CPY",
    [Opcode_INC_ZeroPage ]   = "INC",
    [Opcode_INC_ZeroPageX]   = "INC",
    [Opcode_INC_Absolute ]   = "INC",
    [Opcode_INC_AbsoluteX]   = "INC",
    [Opcode_INX            ] = "INX",
    [Opcode_INY            ] = "INY",    
    [Opcode_DEC_ZeroPage   ] = "DEC",
    [Opcode_DEC_ZeroPageX  ] = "DEC",
    [Opcode_DEC_Absolute   ] = "DEC",
    [Opcode_DEC_AbsoluteX  ] = "DEC",
    [Opcode_DEX            ] = "DEX",    
    [Opcode_DEY            ] = "DEY",    
    [Opcode_ASL_Accumulator] = "ASL",
    [Opcode_ASL_ZeroPage   ] = "ASL",
    [Opcode_ASL_ZeroPageX  ] = "ASL",
    [Opcode_ASL_Absolute   ] = "ASL",
    [Opcode_ASL_AbsoluteX  ] = "ASL",
    [Opcode_LSR_Accumulator] = "LSR",
    [Opcode_LSR_ZeroPage   ] = "LSR",
    [Opcode_LSR_ZeroPageX  ] = "LSR",
    [Opcode_LSR_Absolute   ] = "LSR",
    [Opcode_LSR_AbsoluteX  ] = "LSR",
    [Opcode_ROL_Accumulator] = "ROL",
    [Opcode_ROL_ZeroPage   ] = "ROL",
    [Opcode_ROL_ZeroPageX  ] = "ROL",
    [Opcode_ROL_Absolute   ] = "ROL",
    [Opcode_ROL_AbsoluteX  ] = "ROL",
    [Opcode_ROR_Accumulator] = "ROR",
    [Opcode_ROR_ZeroPage   ] = "ROR",
    [Opcode_ROR_ZeroPageX  ] = "ROR",
    [Opcode_ROR_Absolute   ] = "ROR",
    [Opcode_ROR_AbsoluteX  ] = "ROR",
    [Opcode_JMP_Absolute   ] = "JMP",
    [Opcode_JMP_Indirect   ] = "JMP",
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
    [Opcode_JSR            ] = Add_Absolute ,       
    [Opcode_RTS            ] = Add_Implied ,       
    [Opcode_BCC            ] = Add_Relative,        
    [Opcode_BCS            ] = Add_Relative,        
    [Opcode_BEQ            ] = Add_Relative,        
    [Opcode_BMI            ] = Add_Relative,        
    [Opcode_BNE            ] = Add_Relative,        
    [Opcode_BPL            ] = Add_Relative,        
    [Opcode_BVC            ] = Add_Relative,        
    [Opcode_BVS            ] = Add_Relative,        
    [Opcode_CLC            ] = Add_Implied ,       
    [Opcode_CLD            ] = Add_Implied ,       
    [Opcode_CLI            ] = Add_Implied ,       
    [Opcode_CLV            ] = Add_Implied ,       
    [Opcode_SEC            ] = Add_Implied ,       
    [Opcode_SED            ] = Add_Implied ,       
    [Opcode_SEI            ] = Add_Implied ,       
    [Opcode_BRK            ] = Add_Brk     ,   
    [Opcode_NOP            ] = Add_Implied ,       
    [Opcode_RTI            ] = Add_Implied ,       
};

void CPU_dump_cpu(CPU * cpu, FILE * file)
{
    if(!file) file = stdout;
    fprintf(file, "CPU: {\n");
    fprintf(file, "A: %d (0x%.2x)\n", cpu->A, cpu->A);
    fprintf(file, "X: %d (0x%.2x)\n", cpu->X, cpu->X);
    fprintf(file, "Y: %d (0x%.2x)\n", cpu->Y, cpu->Y);
    fprintf(file, "PC: 0x%.4x\n", cpu->PC);
    fprintf(file, "SP: 0x%.2x\n", cpu->SP);
    fprintf(file, "P (0x%.2x) {\n", cpu->P);
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

        switch (opcode_to_Addressing_mode[cpu->memory[addr]])
        {
        case Add_non: // unknown opcode
            fprintf(file, "0x%.4x:\t???\t\t <byte: 0x%.2x>\n", addr, cpu->memory[addr]);
            break;
        case Add_Brk: // 1 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr++;
            fprintf(file, "\t$%.2x\n", cpu->memory[addr]);
            break;
        case Add_Relative: // 1 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr++;
            fprintf(file, "\t$%.2x\t <sign: %d>\n", cpu->memory[addr], cpu->memory[addr]);  // signed
            break;
        case Add_Implied: // 0 operand
            fprintf(file, "0x%.4x:\t%s\n", addr, opcode_to_cstr[cpu->memory[addr]]);
            break;
        case Add_Immediate: // 1 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr++;
            fprintf(file, "\t#$%.2x\t <deci: %u, sign: %d>\n", cpu->memory[addr], cpu->memory[addr], cpu->memory[addr]);
            break;
        case Add_Accumulator: // 0 operand
            fprintf(file, "0x%.4x:\t%s\n", addr, opcode_to_cstr[cpu->memory[addr]]);
            break;
        case Add_Absolute: // 2 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr += 2;
            fprintf(file, "\t$%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x\n", cpu->memory[addr]);
            addr++;
            break;
        case Add_AbsoluteX: // 2 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr += 2;
            fprintf(file, "\t$%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x,X\n", cpu->memory[addr]);
            addr++;
            break;
        case Add_AbsoluteY: // 2 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr += 2;
            fprintf(file, "\t$%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x,Y\n", cpu->memory[addr]);
            addr++;
            break;
        case Add_ZeroPage: // 1 operand
        fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr++;
            fprintf(file, "\t$%.2x\n", cpu->memory[addr]);
            break;
        case Add_ZeroPageX: // 1 operand
        fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr++;
            fprintf(file, "\t$%.2x,X\n", cpu->memory[addr]);
            break;
        case Add_ZeroPageY: // 1 operand
        fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr++;
            fprintf(file, "\t$%.2x,Y\n", cpu->memory[addr]);
            break;
        case Add_Indirect: // 2 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr += 2;
            fprintf(file, "\t($%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x)\n", cpu->memory[addr]);
            addr++;
            break;
        case Add_IndirectX: // 2 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr += 2;
            fprintf(file, "\t($%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x,X)\n", cpu->memory[addr]);
            addr++;
            break;
        case Add_IndirectY: // 2 operand
            fprintf(file, "0x%.4x:\t%s", addr, opcode_to_cstr[cpu->memory[addr]]);
            addr += 2;
            fprintf(file, "\t($%.2x", cpu->memory[addr]);
            addr--;
            fprintf(file, "%.2x),Y\n", cpu->memory[addr]);
            addr++;
            break;
        
        default: // error
            fprintf(stderr, COLOR_RED "ERROR: unkown Addressing mode number: %d for opcode: 0x%.2x\n" COLOR_RESET,
                opcode_to_Addressing_mode[cpu->memory[addr]],
                cpu->memory[addr]
            );
            break;
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
            fprintf(stderr, COLOR_RED "ERROR: unknown cpu flag %c\n" COLOR_RESET, flag);
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
        cpu->memory[STACK_START + cpu->SP--] = HIGH_BYTE(cpu->PC);
        cpu->memory[STACK_START + cpu->SP--] = LOW_BYTE(cpu->PC);
        break;
    default:
        fprintf(stderr, COLOR_RED "ERROR: non legal push operation for register `%c`\n" COLOR_RESET, reg);
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
        cpu->P = cpu->memory[++cpu->SP + STACK_START];
        CPU_onFlag(cpu, 'u'); // always on
        break;
    case 'C': // for PC
        low  = cpu->memory[STACK_START + ++cpu->SP];
        high = cpu->memory[STACK_START + ++cpu->SP];
        cpu->PC = (high << 8) | low;
        break;
    default:
        fprintf(stderr, COLOR_RED "ERROR: non legal pop operation for register `%c`\n" COLOR_RESET, reg);
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
            fprintf(stderr, COLOR_RED "ERROR: unknown cpu flag %c\n" COLOR_RESET, flag);
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
            fprintf(stderr, COLOR_RED "ERROR: unknown cpu flag %c\n" COLOR_RESET, flag);
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
    CPU_tick(cpu, 2);
}

// done at the hardware level not using normal cpu instructions.
// call wan the reset input is set low, not an interrupt!  
void CPU_reset(CPU *cpu)
{
    cpu->SP = RESET_SP_REGISTER;
    CPU_onFlag(cpu, 'u');
    CPU_onFlag(cpu, 'i');
    CPU_offFlag(cpu, 'd');

    CPU_tick(cpu, 7);

    word bus;
    bus = cpu->memory[0xfffc];
    bus += cpu->memory[0xfffd] << 8;
    cpu->PC = bus;
}

#ifdef _WIN64
#include <conio.h>
#endif

void CPU_run(CPU* cpu, bool is_debug){

    while(true){

        // for debug
        if (is_debug) is_debug = CPU_debug(cpu);

#ifdef _WIN64
        // note: not like the apple-1 I/O system i use 2 ctrl regs and 2 data regs for I/O
        // Consider this part as a hardware implementation of the keyboard and the screen.
        // keyboard:
        char C_terminal_input;
        if (_kbhit()) {                                         // check if a key was pressed (non-blocking)
            C_terminal_input = _getch();                        // read character without echo
            if(C_terminal_input == 0x1B) return;                // ESC for debug
            else if (C_terminal_input == 0x09) CPU_irq(cpu);    // Ctrl-I
            else if (C_terminal_input == 0x0E) CPU_nmi(cpu);    // Ctrl-N
            else if (C_terminal_input == 0x12) CPU_reset(cpu);  // Ctrl-R
            else{
                cpu->memory[0xd010] = C_terminal_input;         // put the char on the MMIO register
                cpu->memory[0xd011] = 1;                        // keyboard_ctrl is set to 1
            } 
        }
        // screen:
        char C_terminal_output;
        if(cpu->memory[0xd013] == 1){                   // if screen_ctrl is set to 1
            C_terminal_output = cpu->memory[0xd012];    // puts on the screen the byte from screen_data
            cpu->memory[0xd013] = 0;                    // set screen_ctrl to 0
            putchar(C_terminal_output);                 // print the char
        } 

#endif

        // fetch:
        Opcode opcode = cpu->memory[cpu->PC++];
        if(opcode == Opcode_HLT) return; // for debug

        // decode:
        Instruction instruction = Opcode_to_Instruction_table[opcode];
        if(!instruction){
            CPU_invalid_opcode(cpu, opcode);
        }
        
        // execute:
        instruction(cpu);
    }
}

void CPU_updateFlags(CPU* cpu, char reg, char flag, byte old_value, byte operand){
    byte new_value = 0;
    switch (reg)
    {
    case 'A':
        new_value = cpu->A;
        break;
    case 'X':
        new_value = cpu->X;
        break;
    case 'Y':
        new_value = cpu->Y;
        break;
    
    default:
        fprintf(stderr, COLOR_RED "ERROR: non legal register: `%c` for appdate flags\n" COLOR_RESET, reg);
        return;
    }

    switch (flag)
    {
    case 'z':
        IS_ZERO(new_value) ? CPU_onFlag(cpu,'z') : CPU_offFlag(cpu,'z'); break;
    case 'v':
        IS_OVERFLOW(old_value, operand) ? CPU_onFlag(cpu,'v') : CPU_offFlag(cpu,'v'); break;
    case 'c':
        IS_CARRY(old_value, operand) ? CPU_onFlag(cpu,'c') : CPU_offFlag(cpu,'c'); break;
    case 'n':
        IS_NEGATIVE(new_value) ? CPU_onFlag(cpu,'n') : CPU_offFlag(cpu,'n'); break;
    default:
        fprintf(stderr, COLOR_RED "ERROR: non legal flag: `%c` for appdate flags\n" COLOR_RESET, flag);
        break;
    }
}

bool CPU_debug(CPU* cpu){
    char input = 0;
    printf("A = 0x%.2x, X = 0x%.2x, Y = 0x%.2x, SP = 0x%.2x, PC = ", cpu->A, cpu->X, cpu->Y, cpu->SP);
    CPU_dumpProgram(cpu, cpu->PC, 1, stdout);
    while(true){
        fflush(stdin);
        input = getchar();
        switch (input)
        {
        case 's':
            return true;
        case 'p':
            printf("P {");
            printf("  n: %d", CPU_getFlag(cpu, 'n'));
            printf("  v: %d", CPU_getFlag(cpu, 'v'));
            printf("  u: %d", CPU_getFlag(cpu, 'u'));
            printf("  b: %d", CPU_getFlag(cpu, 'b'));
            printf("  d: %d", CPU_getFlag(cpu, 'd'));
            printf("  i: %d", CPU_getFlag(cpu, 'i'));
            printf("  z: %d", CPU_getFlag(cpu, 'z'));
            printf("  c: %d", CPU_getFlag(cpu, 'c'));
            printf("  }\n");
            break;
        case 'd':
            CPU_dump_stack(cpu, 0);
            break;
        case 'q':
            return false;
            
            
        default:
            fprintf(stderr, COLOR_RED "ERROR: unknown debug flag: `%c`\n" COLOR_RESET, input);
            break;
        }
    }
}

void CPU_irq(CPU *cpu)
{
    // if interrupt flag disable return
    if(CPU_getFlag(cpu, 'i') == 1){
        return;
    }

    // pull irq handler addres from the interrupt vector 
    word addr = 0;
    addr += cpu->memory[0xfffe];
    addr += cpu->memory[0xffff] << 8;

    // push return addres to stack
    CPU_push(cpu, 'C');

    // push P(flags on the stack)
    CPU_offFlag(cpu, 'b');
    CPU_push(cpu, 'P');

    // jump to handler
    cpu->PC = addr;

    CPU_onFlag(cpu, 'i');
    CPU_tick(cpu, 7);

}

void CPU_nmi(CPU *cpu)
{

    // pull nmi handler addres from the nmi vector 
    word addr = 0;
    addr += cpu->memory[0xfffa];
    addr += cpu->memory[0xfffb] << 8;

    // push return addres to stack
    CPU_push(cpu, 'C');

    // push P(flags on the stack)
    CPU_offFlag(cpu, 'b');
    CPU_push(cpu, 'P');

    // jump to handler
    cpu->PC = addr;

    CPU_onFlag(cpu, 'i');
    CPU_tick(cpu, 7);
}
