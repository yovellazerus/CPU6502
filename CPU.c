
#include "CPU.h"

void CPU_init(CPU* cpu, const char* name){
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->PC = 0;
    cpu->SP = 0;
    cpu->Flags = 0;
    cpu->cycles = 0;
    cpu->name = name;
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
            cpu->Flags &= 0b11111110;
            break;
        case 'v':
            cpu->Flags &= 0b11111101;
            break;
        case 'b':
            cpu->Flags &= 0b11111011;;
            break;
        case 'd':
            cpu->Flags &= 0b11110111;
            break;
        case 'i':
            cpu->Flags &= 0b11101111;
            break;
        case 'z':
            cpu->Flags &= 0b11011111;
            break;
        case 'c':
            cpu->Flags &= 0b10111111;
            break;
        default:
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
    }
    return true;
}

void CPU_dump(CPU* cpu, FILE* stream){
    fprintf(stream, "CPU: %s {\n", cpu->name ? cpu->name : "CPU1");
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
    fprintf(stream, " cycles: %zu\n", cpu->cycles);
    fprintf(stream, "}\n");
}

void CPU_execute(CPU* cpu, Memory* mem){
    byte opcode = mem->data[cpu->PC];
    cpu->PC++;
    operation op = operation_table[opcode];
    if(op){
        op(cpu, mem);
    }
    else{
        CPU_invalid_opcode(cpu, opcode);
    }
}

void CPU_tick(CPU* cpu, size_t amount){
    cpu->cycles += amount;
    // TODO: add sleep or something to simulate real cpu execution time
}

void CPU_invalid_opcode(CPU* cpu, byte opcode){
    // TODO: Treat the exception in a real way
    fprintf(stderr, "ERROR: unknown opcode 0x%.2x\n", opcode);
}