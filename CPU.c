
#include "CPU.h"

void CPU_init(CPU* cpu){
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->PC = 0;
    cpu->SP = 0;
    cpu->Flags = 0;
    cpu->Cycles = 0;
}

bool CPU_getFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            return (1 & (cpu->Flags >> 0));
            break;
        case 'v':
            return (1 & (cpu->Flags >> 1));
            break;
        case 'b':
            return (1 & (cpu->Flags >> 2));
            break;
        case 'd':
            return (1 & (cpu->Flags >> 3));
            break;
        case 'i':
            return (1 & (cpu->Flags >> 4));
            break;
        case 'z':
            return (1 & (cpu->Flags >> 5));
            break;
        case 'c':
            return (1 & (cpu->Flags >> 6));
            break;
        default:
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
    }
}

bool CPU_onFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            cpu->Flags |= 0b00000001;
            break;
        case 'v':
            cpu->Flags |= 0b00000010;
            break;
        case 'b':
            cpu->Flags |= 0b00000100;;
            break;
        case 'd':
            cpu->Flags |= 0b00001000;
            break;
        case 'i':
            cpu->Flags |= 0b00010000;
            break;
        case 'z':
            cpu->Flags |= 0b00100000;
            break;
        case 'c':
            cpu->Flags |= 0b01000000;
            break;
        default:
            fprintf(stderr, "ERROR: unknown cpu flag '%c'\n", flag);
            return false;
    }
    return true;
}

bool CPU_offFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            cpu->Flags |= 0b11111110;
            break;
        case 'v':
            cpu->Flags |= 0b11111101;
            break;
        case 'b':
            cpu->Flags |= 0b11111011;;
            break;
        case 'd':
            cpu->Flags |= 0b11110111;
            break;
        case 'i':
            cpu->Flags |= 0b11101111;
            break;
        case 'z':
            cpu->Flags |= 0b11011111;
            break;
        case 'c':
            cpu->Flags |= 0b10111111;
            break;
        default:
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
    }
    return true;
}

void CPU_dump(CPU* cpu, FILE* stream){
    fprintf(stream, "CPU {\n");
    fprintf(stream, "A: %d\n", cpu->A);
    fprintf(stream, "X: %d\n", cpu->X);
    fprintf(stream, "Y: %d\n", cpu->Y);
    fprintf(stream, "PC: 0x%.4x\n", cpu->PC);
    fprintf(stream, "SP: 0x%.2x\n", cpu->SP);
    fprintf(stream, "Flags {\n");
    fprintf(stream, "    n: %d\n", CPU_getFlag(cpu, 'n'));
    fprintf(stream, "    v: %d\n", CPU_getFlag(cpu, 'v'));
    fprintf(stream, "    b: %d\n", CPU_getFlag(cpu, 'b'));
    fprintf(stream, "    d: %d\n", CPU_getFlag(cpu, 'd'));
    fprintf(stream, "    i: %d\n", CPU_getFlag(cpu, 'i'));
    fprintf(stream, "    z: %d\n", CPU_getFlag(cpu, 'z'));
    fprintf(stream, "    c: %d\n", CPU_getFlag(cpu, 'c'));
    fprintf(stream, "   }\n");
    fprintf(stream, " Cycles: %zu\n", cpu->Cycles);
    fprintf(stream, "}\n");
}

void CPU_execute(CPU* cpu, Memory* mem){
    byte opcode = mem->data[cpu->PC];
    cpu->PC++;
    instruction inst = instruction_table[opcode];
    if(inst){
        inst(cpu, mem);
    }
    else{
        fprintf(stderr, "ERROR: unknown opcode 0x%.2x\n", opcode);
        return;
    }
}

// instructions:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void instruction_LDA_imm(CPU *cpu, Memory *mem)
{
    
}

void instruction_LDA_abs(CPU *cpu, Memory *mem)
{
    
}

void instruction_LDA_abs_X(CPU *cpu, Memory *mem)
{
}

void instruction_LDA_abs_Y(CPU *cpu, Memory *mem)
{
}

void instruction_LDA_zp(CPU *cpu, Memory *mem)
{
}

void instruction_LDA_zp_X(CPU *cpu, Memory *mem)
{
}

void instruction_LDA_indirect_X(CPU *cpu, Memory *mem)
{
}

void instruction_LDA_indirect_Y(CPU *cpu, Memory *mem)
{
}
