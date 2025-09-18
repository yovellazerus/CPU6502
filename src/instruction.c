
#include "../include/instruction.h"

// TODO: fix bugs in zero page mods
static void helper_load(CPU* cpu, char reg, Addressing_mode amod){
    byte imm = 0;
    word addr = 0;
    byte old_cpu = 0;
    byte indirect = 0;
    if(reg == 'A'){
        old_cpu = cpu->A;
        switch (amod)
        {
        case Add_Immediate:
            imm = cpu->memory[cpu->PC++];
            cpu->A = imm;
            CPU_tick(cpu, 2); 
            break;
        case Add_Absolute:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->A = cpu->memory[addr];
            CPU_tick(cpu, 4);
            break;
        case Add_AbsoluteX:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->A = cpu->memory[addr + cpu->X];
            CPU_tick(cpu, 4);
            if(HIGH_BYTE(cpu->X + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
            break;
        case Add_AbsoluteY:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->A = cpu->memory[addr + cpu->Y];
            CPU_tick(cpu, 4);
            if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
            break;
        case Add_ZeroPage:
            addr = cpu->memory[cpu->PC++];
            cpu->A = cpu->memory[addr];
            CPU_tick(cpu, 3);
            break;
        case Add_ZeroPageX:
            addr = cpu->memory[cpu->PC++];
            cpu->A = cpu->memory[(addr + cpu->X) & 0xFF];
            CPU_tick(cpu, 4);
            break;
        case Add_IndirectX:
            indirect = cpu->memory[cpu->PC++];
            indirect += cpu->X;
            addr = cpu->memory[indirect];
            addr += cpu->memory[indirect + 1] << 8;
            cpu->A = cpu->memory[addr];
            CPU_tick(cpu, 6);
            break;
        case Add_IndirectY:
            indirect = cpu->memory[cpu->PC++];
            addr = cpu->memory[indirect];
            addr += cpu->memory[indirect + 1] << 8;
            cpu->A = cpu->memory[addr + cpu->Y];
            CPU_tick(cpu, 5);
            if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
            break;
        
        default:
            fprintf(stderr, COLOR_RED "ERROR: invalid Addresing Mod: `%s` for lda instraction\n" COLOR_RESET, 
                    amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
            break;
        }
        CPU_updateFlags(cpu, 'A', 'z', old_cpu, 0);
        CPU_updateFlags(cpu, 'A', 'n', old_cpu, 0);
    }
    else if(reg == 'X'){
        old_cpu = cpu->X;
        switch (amod)
        {
        case Add_Immediate:
            imm = cpu->memory[cpu->PC++];
            cpu->X = imm;
            CPU_tick(cpu, 2); 
            break;
        case Add_Absolute:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->X = cpu->memory[addr];
            CPU_tick(cpu, 4);
            break;
        case Add_AbsoluteY:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->X = cpu->memory[addr + cpu->Y];
            CPU_tick(cpu, 4);
            if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
            break;
        case Add_ZeroPage:
            addr = cpu->memory[cpu->PC++];
            cpu->X = cpu->memory[addr];
            CPU_tick(cpu, 3);
            break;
        case Add_ZeroPageY:
            addr = cpu->memory[cpu->PC++];
            cpu->X = cpu->memory[(addr + cpu->Y) & 0xFF];
            CPU_tick(cpu, 4);
            break;
    
        default: // error
            fprintf(stderr, COLOR_RED "ERROR: invalid Addresing Mod: `%s` for ldx instraction\n" COLOR_RESET, 
                    amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
            break;
        }
        CPU_updateFlags(cpu, 'X', 'z', old_cpu, 0);
        CPU_updateFlags(cpu, 'X', 'n', old_cpu, 0);
    }
    else if(reg == 'Y'){
        old_cpu = cpu->Y;
        switch (amod)
        {
        case Add_Immediate:
            imm = cpu->memory[cpu->PC++];
            cpu->Y = imm;
            CPU_tick(cpu, 2); 
            break;
        case Add_Absolute:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->Y = cpu->memory[addr];
            CPU_tick(cpu, 4);
            break;
        case Add_AbsoluteX:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->Y = cpu->memory[addr + cpu->X];
            CPU_tick(cpu, 4);
            if(HIGH_BYTE(cpu->X + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
            break;
        case Add_ZeroPage:
            addr = cpu->memory[cpu->PC++];
            cpu->Y = cpu->memory[addr];;
            CPU_tick(cpu, 3);
            break;
        case Add_ZeroPageX:
            addr = cpu->memory[cpu->PC++];
            cpu->Y = cpu->memory[(addr + cpu->X) & 0xFF];
            CPU_tick(cpu, 4);
            break;
    
        default: // error
            fprintf(stderr, COLOR_RED "ERROR: invalid Addresing Mod: `%s` for ldy instraction\n" COLOR_RESET, 
                    amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
            break;
        }
        CPU_updateFlags(cpu, 'Y', 'z', old_cpu, 0);
        CPU_updateFlags(cpu, 'Y', 'n', old_cpu, 0);
    }
    else{
        fprintf(stderr, COLOR_RED "ERROR: invalid register: `%c` for load operation\n" COLOR_RESET, reg);
    }
}
// TODO: fix bugs in zero page mods
static void helper_store(CPU* cpu, char reg, Addressing_mode amod){
    byte imm = 0;
    word addr = 0;
    byte old_cpu = 0;
    byte indirect = 0;
    if(reg == 'A'){
        old_cpu = cpu->A;
        switch (amod)
        {
        case Add_Absolute:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->memory[addr] = cpu->A;
            CPU_tick(cpu, 4);
            break;
        case Add_AbsoluteX:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->memory[addr + cpu->X] = cpu->A;
            CPU_tick(cpu, 4);
            break;
        case Add_AbsoluteY:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->memory[addr + cpu->Y] = cpu->A;
            CPU_tick(cpu, 4);
            break;
        case Add_ZeroPage:
            addr = cpu->memory[cpu->PC++];
            cpu->memory[addr] = cpu->A;
            CPU_tick(cpu, 3);
            break;
        case Add_ZeroPageX:
            cpu->memory[cpu->X + cpu->PC++] = cpu->A;
            CPU_tick(cpu, 4);
            break;
        case Add_IndirectX:
            indirect = cpu->memory[cpu->PC++];
            indirect += cpu->X;
            addr = cpu->memory[indirect];
            addr += cpu->memory[indirect + 1] << 8;
            cpu->memory[addr] = cpu->A;
            CPU_tick(cpu, 6);
            break;
        case Add_IndirectY:
            indirect = cpu->memory[cpu->PC++];
            addr = cpu->memory[indirect];
            addr += cpu->memory[indirect + 1] << 8;
            cpu->memory[addr + cpu->Y] = cpu->A;
            CPU_tick(cpu, 5);
            break;
        
        default:
            
            fprintf(stderr, "ERROR: invalid Addresing Mod: `%s` for sta instraction\n", 
                    amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
            
            break;
        }
    }
    else if(reg == 'X'){
        old_cpu = cpu->X;
        switch (amod)
        {
        case Add_Absolute:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->memory[addr] = cpu->X;
            CPU_tick(cpu, 4);
            break;
        case Add_ZeroPage:
            addr = cpu->memory[cpu->PC++];
            cpu->memory[addr] = cpu->X;
            CPU_tick(cpu, 3);
            break;
        case Add_ZeroPageY:
            cpu->memory[cpu->Y + cpu->PC++] = cpu->X;
            CPU_tick(cpu, 4);
            break;
    
        default: // error
            
            fprintf(stderr, "ERROR: invalid Addresing Mod: `%s` for stx instraction\n", 
                    amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
            
            break;
        }
    }
    else if(reg == 'Y'){
        old_cpu = cpu->Y;
        switch (amod)
        {
        case Add_Absolute:
            addr = cpu->memory[cpu->PC++];
            addr += cpu->memory[cpu->PC++] << 8;
            cpu->memory[addr] = cpu->Y;
            CPU_tick(cpu, 4);
            break;
        case Add_ZeroPage:
            addr = cpu->memory[cpu->PC++];
            cpu->memory[addr] = cpu->Y;
            CPU_tick(cpu, 3);
            break;
        case Add_ZeroPageX:
            cpu->memory[cpu->X + cpu->PC++] = cpu->Y;
            CPU_tick(cpu, 4);
            break;
    
        default: // error
            
            fprintf(stderr, "ERROR: invalid Addresing Mod: `%s` for sty instraction\n", 
                    amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
            
            break;
        }

    }
    else{
        
        fprintf(stderr, "ERROR: invalid register: `%c` for store operation\n", reg);
        
    }
}

static void helper_and(CPU* cpu, Addressing_mode amod){
    byte imm = 0;
    word addr = 0;
    word indirect = 0;
    switch (amod)
    {
    case Add_Immediate:
        imm = cpu->memory[cpu->PC++];
        cpu->A &= imm;
        CPU_tick(cpu, 2);
        break;
    case Add_ZeroPage:
        addr = cpu->memory[cpu->PC++];
        cpu->A &= cpu->memory[addr];
        CPU_tick(cpu, 3);
        break;
    case Add_ZeroPageX:
        addr = (cpu->memory[cpu->PC++] + cpu->X) & 0xFF;
        cpu->A &= cpu->memory[addr];
        CPU_tick(cpu, 4);
        break;
    case Add_Absolute:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A &= cpu->memory[addr];
        CPU_tick(cpu, 4);
        break;
    case Add_AbsoluteX:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A &= cpu->memory[addr + cpu->X];
        CPU_tick(cpu, 4);
        if(HIGH_BYTE(cpu->X + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    case Add_AbsoluteY:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A &= cpu->memory[addr + cpu->Y];
        CPU_tick(cpu, 4);
        if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    case Add_IndirectX:
        indirect = (cpu->memory[cpu->PC++] + cpu->X) & 0xFF;
        addr = cpu->memory[indirect];
        addr |= cpu->memory[(indirect + 1) & 0xFF] << 8;
        cpu->A &= cpu->memory[addr];
        CPU_tick(cpu, 6);
        break;
    case Add_IndirectY:
        indirect = cpu->memory[cpu->PC++];
        addr = cpu->memory[indirect];
        addr += cpu->memory[(indirect + 1) & 0xff] << 8;
        cpu->A &= cpu->memory[addr + cpu->Y];
        CPU_tick(cpu, 5);
        if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    
    default:
        
        fprintf(stderr, "ERROR: invalid Addresing Mod: `%s` for and instruction\n", 
                amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
        
        break;
    }
    CPU_updateFlags(cpu, 'A', 'z', cpu->A, 0);
    CPU_updateFlags(cpu, 'A', 'n', cpu->A, 0);
}

static void helper_eor(CPU* cpu, Addressing_mode amod){
    byte imm = 0;
    word addr = 0;
    word indirect = 0;
    switch (amod)
    {
    case Add_Immediate:
        imm = cpu->memory[cpu->PC++];
        cpu->A ^= imm;
        CPU_tick(cpu, 2);
        break;
    case Add_ZeroPage:
        addr = cpu->memory[cpu->PC++];
        cpu->A ^= cpu->memory[addr];
        CPU_tick(cpu, 3);
        break;
    case Add_ZeroPageX:
        addr = (cpu->memory[cpu->PC++] + cpu->X) & 0xFF;
        cpu->A ^= cpu->memory[addr];
        CPU_tick(cpu, 4);
        break;
    case Add_Absolute:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A ^= cpu->memory[addr];
        CPU_tick(cpu, 4);
        break;
    case Add_AbsoluteX:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A ^= cpu->memory[addr + cpu->X];
        CPU_tick(cpu, 4);
        if(HIGH_BYTE(cpu->X + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    case Add_AbsoluteY:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A ^= cpu->memory[addr + cpu->Y];
        CPU_tick(cpu, 4);
        if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    case Add_IndirectX:
        indirect = (cpu->memory[cpu->PC++] + cpu->X) & 0xFF;
        addr = cpu->memory[indirect];
        addr |= cpu->memory[(indirect + 1) & 0xFF] << 8;
        cpu->A ^= cpu->memory[addr];
        CPU_tick(cpu, 6);
        break;
    case Add_IndirectY:
        indirect = cpu->memory[cpu->PC++];
        addr = cpu->memory[indirect];
        addr |= cpu->memory[(indirect + 1) & 0xff] << 8;
        cpu->A ^= cpu->memory[addr + cpu->Y];
        CPU_tick(cpu, 5);
        if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    
    default:
        
        fprintf(stderr, "ERROR: invalid Addresing Mod: `%s` for eor instruction\n", 
                amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
        
        break;
    }
    CPU_updateFlags(cpu, 'A', 'z', cpu->A, 0);
    CPU_updateFlags(cpu, 'A', 'n', cpu->A, 0);
}

static void helper_ora(CPU* cpu, Addressing_mode amod){
    byte imm = 0;
    word addr = 0;
    word indirect = 0;
    switch (amod)
    {
    case Add_Immediate:
        imm = cpu->memory[cpu->PC++];
        cpu->A |= imm;
        CPU_tick(cpu, 2);
        break;
    case Add_ZeroPage:
        addr = cpu->memory[cpu->PC++];
        cpu->A |= cpu->memory[addr];
        CPU_tick(cpu, 3);
        break;
    case Add_ZeroPageX:
        addr = (cpu->memory[cpu->PC++] + cpu->X) & 0xFF;
        cpu->A |= cpu->memory[addr];
        CPU_tick(cpu, 4);
        break;
    case Add_Absolute:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A |= cpu->memory[addr];
        CPU_tick(cpu, 4);
        break;
    case Add_AbsoluteX:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A |= cpu->memory[addr + cpu->X];
        CPU_tick(cpu, 4);
        if(HIGH_BYTE(cpu->X + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    case Add_AbsoluteY:
        addr = cpu->memory[cpu->PC++];
        addr += cpu->memory[cpu->PC++] << 8;
        cpu->A |= cpu->memory[addr + cpu->Y];
        CPU_tick(cpu, 4);
        if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    case Add_IndirectX:
        indirect = (cpu->memory[cpu->PC++] + cpu->X) & 0xFF;
        addr = cpu->memory[indirect];
        addr |= cpu->memory[(indirect + 1) & 0xFF] << 8;
        cpu->A |= cpu->memory[addr];
        CPU_tick(cpu, 6);
        break;
    case Add_IndirectY:
        indirect = cpu->memory[cpu->PC++];
        addr = cpu->memory[indirect];
        addr |= cpu->memory[(indirect + 1) & 0xff] << 8;
        cpu->A |= cpu->memory[addr + cpu->Y];
        CPU_tick(cpu, 5);
        if(HIGH_BYTE(cpu->Y + addr) != HIGH_BYTE(addr)) CPU_tick(cpu, 1); // page crossed
        break;
    
    default:
        
        fprintf(stderr, "ERROR: invalid Addresing Mod: `%s` for ora instruction\n", 
                amod < count_Add ? Addressing_mode_to_cstr[amod] : Addressing_mode_to_cstr[count_Add]);
        
        break;
    }
    CPU_updateFlags(cpu, 'A', 'z', cpu->A, 0);
    CPU_updateFlags(cpu, 'A', 'n', cpu->A, 0);
}

Instruction Opcode_to_Instruction_table[0xff + 1] = {
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

    // // Stack
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
    // [Opcode_ADC_Immediate]   = instruction_ADC_Immediate,
    // [Opcode_ADC_ZeroPage]    = instruction_ADC_ZeroPage,
    // [Opcode_ADC_ZeroPageX]   = instruction_ADC_ZeroPageX,
    // [Opcode_ADC_Absolute]    = instruction_ADC_Absolute,
    // [Opcode_ADC_AbsoluteX]   = instruction_ADC_AbsoluteX,
    // [Opcode_ADC_AbsoluteY]   = instruction_ADC_AbsoluteY,
    // [Opcode_ADC_IndirectX]   = instruction_ADC_IndirectX,
    // [Opcode_ADC_IndirectY]   = instruction_ADC_IndirectY,

    // [Opcode_SBC_Immediate]   = instruction_SBC_Immediate,
    // [Opcode_SBC_ZeroPage]    = instruction_SBC_ZeroPage,
    // [Opcode_SBC_ZeroPageX]   = instruction_SBC_ZeroPageX,
    // [Opcode_SBC_Absolute]    = instruction_SBC_Absolute,
    // [Opcode_SBC_AbsoluteX]   = instruction_SBC_AbsoluteX,
    // [Opcode_SBC_AbsoluteY]   = instruction_SBC_AbsoluteY,
    // [Opcode_SBC_IndirectX]   = instruction_SBC_IndirectX,
    // [Opcode_SBC_IndirectY]   = instruction_SBC_IndirectY,

    // [Opcode_CMP_Immediate]   = instruction_CMP_Immediate,
    // [Opcode_CMP_ZeroPage]    = instruction_CMP_ZeroPage,
    // [Opcode_CMP_ZeroPageX]   = instruction_CMP_ZeroPageX,
    // [Opcode_CMP_Absolute]    = instruction_CMP_Absolute,
    // [Opcode_CMP_AbsoluteX]   = instruction_CMP_AbsoluteX,
    // [Opcode_CMP_AbsoluteY]   = instruction_CMP_AbsoluteY,
    // [Opcode_CMP_IndirectX]   = instruction_CMP_IndirectX,
    // [Opcode_CMP_IndirectY]   = instruction_CMP_IndirectY,

    // [Opcode_CPX_Immediate]   = instruction_CPX_Immediate,
    // [Opcode_CPX_ZeroPage]    = instruction_CPX_ZeroPage,
    // [Opcode_CPX_Absolute]    = instruction_CPX_Absolute,

    // [Opcode_CPY_Immediate]   = instruction_CPY_Immediate,
    // [Opcode_CPY_ZeroPage]    = instruction_CPY_ZeroPage,
    // [Opcode_CPY_Absolute]    = instruction_CPY_Absolute,

    // // Increments / Decrements
    // [Opcode_INC_ZeroPage]    = instruction_INC_ZeroPage,
    // [Opcode_INC_ZeroPageX]   = instruction_INC_ZeroPageX,
    // [Opcode_INC_Absolute]    = instruction_INC_Absolute,
    // [Opcode_INC_AbsoluteX]   = instruction_INC_AbsoluteX,

    // [Opcode_INX]             = instruction_INX,
    // [Opcode_INY]             = instruction_INY,

    // [Opcode_DEC_ZeroPage]    = instruction_DEC_ZeroPage,
    // [Opcode_DEC_ZeroPageX]   = instruction_DEC_ZeroPageX,
    // [Opcode_DEC_Absolute]    = instruction_DEC_Absolute,
    // [Opcode_DEC_AbsoluteX]   = instruction_DEC_AbsoluteX,

    // [Opcode_DEX]             = instruction_DEX,
    // [Opcode_DEY]             = instruction_DEY,

    // // Shifts / Rotates
    // [Opcode_ASL_Accumulator] = instruction_ASL_Accumulator,
    // [Opcode_ASL_ZeroPage]    = instruction_ASL_ZeroPage,
    // [Opcode_ASL_ZeroPageX]   = instruction_ASL_ZeroPageX,
    // [Opcode_ASL_Absolute]    = instruction_ASL_Absolute,
    // [Opcode_ASL_AbsoluteX]   = instruction_ASL_AbsoluteX,

    // [Opcode_LSR_Accumulator] = instruction_LSR_Accumulator,
    // [Opcode_LSR_ZeroPage]    = instruction_LSR_ZeroPage,
    // [Opcode_LSR_ZeroPageX]   = instruction_LSR_ZeroPageX,
    // [Opcode_LSR_Absolute]    = instruction_LSR_Absolute,
    // [Opcode_LSR_AbsoluteX]   = instruction_LSR_AbsoluteX,

    // [Opcode_ROL_Accumulator] = instruction_ROL_Accumulator,
    // [Opcode_ROL_ZeroPage]    = instruction_ROL_ZeroPage,
    // [Opcode_ROL_ZeroPageX]   = instruction_ROL_ZeroPageX,
    // [Opcode_ROL_Absolute]    = instruction_ROL_Absolute,
    // [Opcode_ROL_AbsoluteX]   = instruction_ROL_AbsoluteX,

    // [Opcode_ROR_Accumulator] = instruction_ROR_Accumulator,
    // [Opcode_ROR_ZeroPage]    = instruction_ROR_ZeroPage,
    // [Opcode_ROR_ZeroPageX]   = instruction_ROR_ZeroPageX,
    // [Opcode_ROR_Absolute]    = instruction_ROR_Absolute,
    // [Opcode_ROR_AbsoluteX]   = instruction_ROR_AbsoluteX,

    // Jumps & Calls
    [Opcode_JMP_Absolute]    = instruction_JMP_Absolute,
    [Opcode_JMP_Indirect]    = instruction_JMP_Indirect,
    [Opcode_JSR]             = instruction_JSR,
    [Opcode_RTS]             = instruction_RTS,

    // // Branches
    // [Opcode_BCC]             = instruction_BCC,
    // [Opcode_BCS]             = instruction_BCS,
    // [Opcode_BEQ]             = instruction_BEQ,
    // [Opcode_BMI]             = instruction_BMI,
    // [Opcode_BNE]             = instruction_BNE,
    // [Opcode_BPL]             = instruction_BPL,
    // [Opcode_BVC]             = instruction_BVC,
    // [Opcode_BVS]             = instruction_BVS,

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

    // for debug
    [Opcode_HLT]             = instruction_HLT,

};

void instruction_LDA_Immediate(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_Immediate);
}
void instruction_LDA_ZeroPage(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_ZeroPage);

}
void instruction_LDA_ZeroPageX(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_ZeroPageX);

}
void instruction_LDA_Absolute(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_ZeroPageY);

}
void instruction_LDA_AbsoluteX(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_AbsoluteX);

}
void instruction_LDA_AbsoluteY(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_AbsoluteY);

}
void instruction_LDA_IndirectX(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_IndirectX);

}
void instruction_LDA_IndirectY(CPU* cpu) 
{
    helper_load(cpu, 'A', Add_IndirectY);

}

void instruction_LDX_Immediate(CPU* cpu)
{
    helper_load(cpu, 'X', Add_Immediate);

}
void instruction_LDX_ZeroPage(CPU* cpu)
{
    helper_load(cpu, 'X', Add_ZeroPage);

}
void instruction_LDX_ZeroPageY(CPU* cpu)
{
    helper_load(cpu, 'X', Add_ZeroPageY);

}
void instruction_LDX_Absolute(CPU* cpu)
{
    helper_load(cpu, 'X', Add_Absolute);

}
void instruction_LDX_AbsoluteY(CPU* cpu)
{
    helper_load(cpu, 'X', Add_AbsoluteY);

}

void instruction_LDY_Immediate(CPU* cpu)
{
    helper_load(cpu, 'Y', Add_Immediate);

}
void instruction_LDY_ZeroPage(CPU* cpu)
{
    helper_load(cpu, 'Y', Add_ZeroPage);

}
void instruction_LDY_ZeroPageX(CPU* cpu)
{
    helper_load(cpu, 'Y', Add_ZeroPageX);

}
void instruction_LDY_Absolute(CPU* cpu)
{
    helper_load(cpu, 'Y', Add_Absolute);

}
void instruction_LDY_AbsoluteX(CPU* cpu)
{
    helper_load(cpu, 'Y', Add_Absolute);

}

void instruction_STA_ZeroPage(CPU* cpu)
{
    helper_store(cpu, 'A', Add_ZeroPage);
}
void instruction_STA_ZeroPageX(CPU* cpu) 
{
    helper_store(cpu, 'A', Add_ZeroPageX);
}
void instruction_STA_Absolute(CPU* cpu)
{
    helper_store(cpu, 'A', Add_Absolute);

}
void instruction_STA_AbsoluteX(CPU* cpu)
{
    helper_store(cpu, 'A', Add_AbsoluteX);

}
void instruction_STA_AbsoluteY(CPU* cpu)
{
    helper_store(cpu, 'A', Add_AbsoluteY);

}
void instruction_STA_IndirectX(CPU* cpu)
{
    helper_store(cpu, 'A', Add_IndirectX);

}
void instruction_STA_IndirectY(CPU* cpu)
{
    helper_store(cpu, 'A', Add_IndirectY);

}

void instruction_STX_ZeroPage(CPU* cpu)
{
    helper_store(cpu, 'X', Add_ZeroPage);

}
void instruction_STX_ZeroPageY(CPU* cpu)
{
    helper_store(cpu, 'X', Add_ZeroPageY);

}
void instruction_STX_Absolute(CPU* cpu)
{
    helper_store(cpu, 'X', Add_Absolute);

}

void instruction_STY_ZeroPage(CPU* cpu)
{
    helper_store(cpu, 'Y', Add_ZeroPage);

}
void instruction_STY_ZeroPageX(CPU* cpu)
{
    helper_store(cpu, 'Y', Add_ZeroPageX);

}
void instruction_STY_Absolute(CPU* cpu)
{
    helper_store(cpu, 'Y', Add_Absolute);

}

// Register Transfers
void instruction_TAX(CPU* cpu)
{
    cpu->X = cpu->A;
    CPU_tick(cpu, 2);
    CPU_updateFlags(cpu, 'X', 'n', 0, 0);
    CPU_updateFlags(cpu, 'X', 'z', 0, 0);

}
void instruction_TAY(CPU* cpu)
{
    cpu->Y = cpu->A;
    CPU_tick(cpu, 2);
    CPU_updateFlags(cpu, 'Y', 'n', 0, 0);
    CPU_updateFlags(cpu, 'Y', 'z', 0, 0);

}
void instruction_TSX(CPU* cpu)
{
    cpu->X = cpu->SP;
    CPU_tick(cpu, 2);
    CPU_updateFlags(cpu, 'X', 'n', 0, 0);
    CPU_updateFlags(cpu, 'X', 'z', 0, 0);

}
void instruction_TXA(CPU* cpu)
{
    cpu->A = cpu->X;
    CPU_tick(cpu, 2);
    CPU_updateFlags(cpu, 'A', 'n', 0, 0);
    CPU_updateFlags(cpu, 'A', 'z', 0, 0);
}
void instruction_TXS(CPU* cpu)
{
    cpu->SP = cpu->X;
    CPU_tick(cpu, 2);
    // not modify the flags register 

}
void instruction_TYA(CPU* cpu)
{
    cpu->A = cpu->Y;
    CPU_tick(cpu, 2);
    CPU_updateFlags(cpu, 'A', 'n', 0, 0);
    CPU_updateFlags(cpu, 'A', 'z', 0, 0);

}

// Stack
void instruction_PHA(CPU* cpu)
{
    CPU_push(cpu, 'A');
    CPU_tick(cpu, 3);

}
void instruction_PHP(CPU* cpu)
{
    CPU_push(cpu, 'P');
    CPU_tick(cpu, 3);

}
void instruction_PLA(CPU* cpu)
{
    CPU_pop(cpu, 'A');
    CPU_tick(cpu, 4);
    CPU_updateFlags(cpu, 'A', 'n', 0, 0);
    CPU_updateFlags(cpu, 'A', 'z', 0, 0);

}
void instruction_PLP(CPU* cpu)
{
    CPU_pop(cpu, 'P');
    CPU_tick(cpu, 4);
}

// Logical
void instruction_AND_Immediate(CPU* cpu)
{
    helper_and(cpu, Add_Immediate);

}
void instruction_AND_ZeroPage(CPU* cpu)
{
    helper_and(cpu, Add_ZeroPage);

}
void instruction_AND_ZeroPageX(CPU* cpu)
{
    helper_and(cpu, Add_ZeroPageX);

}
void instruction_AND_Absolute(CPU* cpu)
{
    helper_and(cpu, Add_Absolute);

}
void instruction_AND_AbsoluteX(CPU* cpu)
{
    helper_and(cpu, Add_AbsoluteX);

}
void instruction_AND_AbsoluteY(CPU* cpu)
{
    helper_and(cpu, Add_AbsoluteY);

}
void instruction_AND_IndirectX(CPU* cpu)
{
    helper_and(cpu, Add_IndirectX);

}
void instruction_AND_IndirectY(CPU* cpu)
{
    helper_and(cpu, Add_IndirectY);

}

void instruction_EOR_Immediate(CPU* cpu)
{
    helper_eor(cpu, Add_Immediate);

}
void instruction_EOR_ZeroPage(CPU* cpu)
{
    helper_eor(cpu, Add_ZeroPage);

}
void instruction_EOR_ZeroPageX(CPU* cpu)
{
    helper_eor(cpu, Add_ZeroPageX);

}
void instruction_EOR_Absolute(CPU* cpu)
{
    helper_eor(cpu, Add_Absolute);

}
void instruction_EOR_AbsoluteX(CPU* cpu)
{
    helper_eor(cpu, Add_AbsoluteX);

}
void instruction_EOR_AbsoluteY(CPU* cpu)
{
    helper_eor(cpu, Add_AbsoluteY);

}
void instruction_EOR_IndirectX(CPU* cpu)
{
    helper_eor(cpu, Add_IndirectX);

}
void instruction_EOR_IndirectY(CPU* cpu)
{
    helper_eor(cpu, Add_IndirectY);

}

void instruction_ORA_Immediate(CPU* cpu)
{
    helper_ora(cpu, Add_Immediate);

}
void instruction_ORA_ZeroPage(CPU* cpu)
{
    helper_ora(cpu, Add_ZeroPage);

}
void instruction_ORA_ZeroPageX(CPU* cpu)
{
    helper_ora(cpu, Add_ZeroPageX);

}
void instruction_ORA_Absolute(CPU* cpu)
{
    helper_ora(cpu, Add_Absolute);

}
void instruction_ORA_AbsoluteX(CPU* cpu)
{
    helper_ora(cpu, Add_AbsoluteX);

}
void instruction_ORA_AbsoluteY(CPU* cpu)
{
    helper_ora(cpu, Add_AbsoluteY);

}
void instruction_ORA_IndirectX(CPU* cpu)
{
    helper_ora(cpu, Add_IndirectX);

}
void instruction_ORA_IndirectY(CPU* cpu)
{
    helper_ora(cpu, Add_IndirectY);

}

void instruction_BIT_ZeroPage(CPU* cpu)
{
    word addr = (cpu->memory[cpu->PC++] & 0xff);
    byte operand = cpu->memory[addr];
    byte res = cpu->A & operand;
    if (res == 0) CPU_onFlag(cpu, 'z'); else CPU_offFlag(cpu, 'z'); 
    cpu->P =  (cpu->P & 0x3f) | (operand & 0xc0); // (cpu->P & 0b00111111) | (operand & 0b11000000)
    CPU_tick(cpu, 3);
}
void instruction_BIT_Absolute(CPU* cpu)
{
    word addr = cpu->memory[cpu->PC++];
    addr += cpu->memory[cpu->PC++] << 8;
    byte operand = cpu->memory[addr];
    byte res = cpu->A & operand;
    if (res == 0) CPU_onFlag(cpu, 'z'); else CPU_offFlag(cpu, 'z'); 
    cpu->P =  (cpu->P & 0x3f) | (operand & 0xc0); // (cpu->P & 0b00111111) | (operand & 0b11000000)
    CPU_tick(cpu, 4);
}

// Arithmetic
void instruction_ADC_Immediate(CPU* cpu)
{
    UNUSED;

}
void instruction_ADC_ZeroPage(CPU* cpu)
{
    byte zero_page_low_byte = cpu->memory[cpu->PC++];
    word addr = ZERO_PAGE_START + zero_page_low_byte;

    byte operand = cpu->memory[addr];
    byte carry_in = CPU_getFlag(cpu, 'c');

    operand = (byte)operand + (byte)carry_in;

    CPU_updateFlags(cpu, 'A', 'z', cpu->A, operand);
    CPU_updateFlags(cpu, 'A', 'n', cpu->A, operand);
    CPU_updateFlags(cpu, 'A', 'c', cpu->A, operand);
    CPU_updateFlags(cpu, 'A', 'v', cpu->A, operand);

    cpu->A += operand;

    CPU_tick(cpu, 3);
}
void instruction_ADC_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_ADC_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_ADC_AbsoluteX(CPU* cpu)
{
    UNUSED;

}
void instruction_ADC_AbsoluteY(CPU* cpu)
{
    UNUSED;

}
void instruction_ADC_IndirectX(CPU* cpu)
{
    UNUSED;

}
void instruction_ADC_IndirectY(CPU* cpu)
{
    UNUSED;

}

void instruction_SBC_Immediate(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_AbsoluteX(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_AbsoluteY(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_IndirectX(CPU* cpu)
{
    UNUSED;

}
void instruction_SBC_IndirectY(CPU* cpu)
{
    UNUSED;

}

void instruction_CMP_Immediate(CPU* cpu){
    UNUSED;

}
void instruction_CMP_ZeroPage(CPU* cpu){
    UNUSED;

}
void instruction_CMP_ZeroPageX(CPU* cpu){
    UNUSED;

}
void instruction_CMP_Absolute(CPU* cpu){
    UNUSED;

}
void instruction_CMP_AbsoluteX(CPU* cpu){
    UNUSED;

}
void instruction_CMP_AbsoluteY(CPU* cpu){
    UNUSED;

}
void instruction_CMP_IndirectX(CPU* cpu){
    UNUSED;

}
void instruction_CMP_IndirectY(CPU* cpu){
    UNUSED;

}

void instruction_CPX_Immediate(CPU* cpu)
{
    UNUSED;

}
void instruction_CPX_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_CPX_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_CPY_Immediate(CPU* cpu)
{
    UNUSED;

}
void instruction_CPY_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_CPY_Absolute(CPU* cpu)
{
    UNUSED;

}

// Increments / Decrements
void instruction_INC_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_INC_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_INC_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_INC_AbsoluteX(CPU* cpu)
{
    UNUSED;

}

void instruction_INX(CPU* cpu){
    UNUSED;

}
void instruction_INY(CPU* cpu){
    UNUSED;

}

void instruction_DEC_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_DEC_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_DEC_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_DEC_AbsoluteX(CPU* cpu)
{
    UNUSED;

}

void instruction_DEX(CPU* cpu)
{
    cpu->X--;
    if(cpu->X == 0) CPU_onFlag(cpu, 'z'); else CPU_offFlag(cpu, 'z');
    if(IS_NEGATIVE(cpu->X)) CPU_onFlag(cpu, 'n'); else CPU_offFlag(cpu, 'n');
    CPU_tick(cpu, 2);
}
void instruction_DEY(CPU* cpu)
{
    UNUSED;

}

// Shifts / Rotates
void instruction_ASL_Accumulator(CPU* cpu)
{
    UNUSED;

}
void instruction_ASL_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_ASL_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_ASL_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_ASL_AbsoluteX(CPU* cpu)
{
    UNUSED;

}

void instruction_LSR_Accumulator(CPU* cpu)
{
    UNUSED;

}
void instruction_LSR_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_LSR_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_LSR_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_LSR_AbsoluteX(CPU* cpu)
{
    UNUSED;

}

void instruction_ROL_Accumulator(CPU* cpu)
{
    UNUSED;

}
void instruction_ROL_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_ROL_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_ROL_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_ROL_AbsoluteX(CPU* cpu)
{
    UNUSED;

}

void instruction_ROR_Accumulator(CPU* cpu)
{
    UNUSED;

}
void instruction_ROR_ZeroPage(CPU* cpu)
{
    UNUSED;

}
void instruction_ROR_ZeroPageX(CPU* cpu)
{
    UNUSED;

}
void instruction_ROR_Absolute(CPU* cpu)
{
    UNUSED;

}
void instruction_ROR_AbsoluteX(CPU* cpu)
{
    UNUSED;

}

// Jumps / Calls
void instruction_JMP_Absolute(CPU* cpu)
{
    word addr = cpu->memory[cpu->PC++];
    addr += cpu->memory[cpu->PC++] << 8;
    cpu->PC = addr;
    CPU_tick(cpu, 3);

}
void instruction_JMP_Indirect(CPU* cpu)
{
    /*
    An original 6502 has does not correctly fetch the target address
    if the indirect vector falls on a page boundary 
    (e.g. $xxFF where xx is any value from $00 to $FF). 
    In this case fetches the LSB from $xxFF as expected but takes the MSB from $xx00. 
    This is fixed in some later chips like the 65SC02
    so for compatibility always ensure the indirect vector is not at the end of the page.
    */

    word indirect = cpu->memory[cpu->PC++];
    indirect += cpu->memory[cpu->PC++] << 8;

    byte lo = cpu->memory[indirect];
    byte hi = cpu->memory[(indirect & 0xFF00) | ((indirect + 1) & 0x00FF)] << 8;

    cpu->PC = lo + hi;
    CPU_tick(cpu, 5);

}
void instruction_JSR(CPU* cpu)
{
    word addr = cpu->memory[cpu->PC++];
    addr += cpu->memory[cpu->PC++] << 8;

    cpu->PC -= 1; // emulate (PC - 1) push
    CPU_push(cpu, 'C');
    cpu->PC = addr;

    CPU_tick(cpu, 6);

}
void instruction_RTS(CPU* cpu)
{
    CPU_pop(cpu, 'C');
    cpu->PC++;
    CPU_tick(cpu, 6);
}

// Branches
void instruction_BCC(CPU* cpu)
{
    UNUSED;

}
void instruction_BCS(CPU* cpu)
{
    UNUSED;

}
void instruction_BEQ(CPU* cpu)
{
    UNUSED;

}
void instruction_BMI(CPU* cpu)
{
    UNUSED;

}
void instruction_BNE(CPU* cpu)
{
    byte disp = cpu->memory[cpu->PC++];
    word old_pc = cpu->PC;
    if(!CPU_getFlag(cpu, 'z')){
        cpu->PC += (int8_t) disp;
        CPU_tick(cpu, 1);
        if(IS_CROSS_PAGES(old_pc, cpu->PC)){
            CPU_tick(cpu, 1);
        }
    }
    CPU_tick(cpu, 2);
}
void instruction_BPL(CPU* cpu)
{
    UNUSED;

}
void instruction_BVC(CPU* cpu)
{
    UNUSED;

}
void instruction_BVS(CPU* cpu)
{
    UNUSED;

}

// Flags
void instruction_CLC(CPU* cpu)
{
    CPU_offFlag(cpu, 'c');
    CPU_tick(cpu, 2);

}
void instruction_CLD(CPU* cpu)
{
    CPU_offFlag(cpu, 'd');
    CPU_tick(cpu, 2);

}
void instruction_CLI(CPU* cpu)
{
    CPU_offFlag(cpu, 'i');
    CPU_tick(cpu, 2);

}
void instruction_CLV(CPU* cpu)
{
    CPU_offFlag(cpu, 'v');
    CPU_tick(cpu, 2);

}
void instruction_SEC(CPU* cpu)
{
    CPU_onFlag(cpu, 'c');
    CPU_tick(cpu, 2);

}
void instruction_SED(CPU* cpu)
{
    CPU_onFlag(cpu, 'd');
    CPU_tick(cpu, 2);

}
void instruction_SEI(CPU* cpu)
{
    CPU_onFlag(cpu, 'i');
    CPU_tick(cpu, 2);

}

// System
void instruction_BRK(CPU* cpu)
{
    word addr = 0;
    addr += cpu->memory[0xfffe];
    addr += cpu->memory[0xffff] << 8;
 
    CPU_onFlag(cpu, 'i');
    cpu->PC += 1;
    CPU_push(cpu, 'C');
    byte old_P = cpu->P;
    CPU_onFlag(cpu, 'b');
    CPU_push(cpu, 'P');
    cpu->P = old_P;

    cpu->PC = addr;

    CPU_tick(cpu, 7);
}
void instruction_NOP(CPU* cpu)
{
    UNUSED;
    CPU_tick(cpu, 2);
}
void instruction_RTI(CPU* cpu)
{
    CPU_pop(cpu, 'P');
    CPU_pop(cpu, 'C');
    CPU_tick(cpu, 6);
}

void instruction_HLT(CPU* cpu){
    cpu->halt = true;
}