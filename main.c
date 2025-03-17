
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

#define MEM_SIZE (64*KB)
#define NUMBER_OF_INSTRUCTIONS 0xff

typedef unsigned char byte;
typedef unsigned short word;

typedef struct memory{
    byte data[MEM_SIZE];  
} Memory;


void Memory_init(Memory* mem){
    for(int i = 0; i < MEM_SIZE; i++){
        mem->data[i] = 0;
    }
}

void Memory_dump(Memory* mem, FILE* stream){
    word line = 0;
    for(int i = 0; i < MEM_SIZE; i++){
        if(i % 8 == 0){
            fprintf(stream, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(stream, " 0x%.2x ", mem->data[i]);
    }
}

typedef struct cpu{
    byte A;
    byte X;
    byte Y;
    word PC;
    byte SP;
    byte Flags;
    
    size_t 	Cycles; 
} Cpu;

void Cpu_init(Cpu* cpu){
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->PC = 0;
    cpu->SP = 0;
    cpu->Flags = 0;
    cpu->Cycles = 0;
}

bool Cpu_getFlag(Cpu* cpu, char flag){
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

bool Cpu_onFlag(Cpu* cpu, char flag){
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

bool Cpu_offFlag(Cpu* cpu, char flag){
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

void Cpu_dump(Cpu* cpu, FILE* stream){
    fprintf(stream, "CPU {\n");
    fprintf(stream, "A: %d\n", cpu->A);
    fprintf(stream, "X: %d\n", cpu->X);
    fprintf(stream, "Y: %d\n", cpu->Y);
    fprintf(stream, "PC: 0x%.4x\n", cpu->PC);
    fprintf(stream, "SP: 0x%.2x\n", cpu->SP);
    fprintf(stream, "Flags {\n");
    fprintf(stream, "    n: %d\n", Cpu_getFlag(cpu, 'n'));
    fprintf(stream, "    v: %d\n", Cpu_getFlag(cpu, 'v'));
    fprintf(stream, "    b: %d\n", Cpu_getFlag(cpu, 'b'));
    fprintf(stream, "    d: %d\n", Cpu_getFlag(cpu, 'd'));
    fprintf(stream, "    i: %d\n", Cpu_getFlag(cpu, 'i'));
    fprintf(stream, "    z: %d\n", Cpu_getFlag(cpu, 'z'));
    fprintf(stream, "    c: %d\n", Cpu_getFlag(cpu, 'c'));
    fprintf(stream, "   }\n");
    fprintf(stream, " Cycles: %zu\n", cpu->Cycles);
    fprintf(stream, "}\n");
}

typedef void (*instruction)(Cpu*, Memory*);

void instruction_LDA_imm(Cpu* cpu, Memory* mem){
    return;
}

instruction instruction_table[NUMBER_OF_INSTRUCTIONS] = {
    // LDA
    [0xa9] = instruction_LDA_imm,
    // [0xad] = instruction_LDA_abs,
    // [0xbd] = instruction_LDA_abs_X,
    // [0xb9] = instruction_LDA_abs_Y,
    // [0xa5] = instruction_LDA_zp,
    // [0xb5] = instruction_LDA_zp_X,
    // [0xa1] = instruction_LDA_indirect_X,
    // [0xb1] = instruction_LDA_indirect_Y,
};

void Cpu_execute(Cpu* cpu, Memory* mem){
    byte opcode = mem->data[cpu->PC];
    cpu->PC++;
    instruction ins = instruction_table[opcode];
    if(ins){
        ins(cpu, mem);
    }
    else{
        fprintf(stderr, "ERROR: unknown opcode 0x%.2x\n", opcode);
        return;
    }
}

int main()
{
    FILE* memory_file = fopen("memory.txt", "w");
    if(!memory_file){
        perror("memory.txt");
        return 1;
    }
    FILE* cpu_file = fopen("cpu.txt", "w");
    if(!cpu_file){
        fclose(cpu_file);
        perror("cpu.txt");
        return 1;
    }
    Memory memory;
    Cpu cpu;
    Memory_init(&memory);
    Cpu_init(&cpu);
    
    Cpu_execute(&cpu, &memory);
    
    Memory_dump(&memory, memory_file);
    Cpu_onFlag(&cpu, 'i');
    Cpu_dump(&cpu, cpu_file);
    
    
    fclose(cpu_file);
    fclose(memory_file);
    
    return 0;
}

