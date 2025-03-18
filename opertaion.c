
#include "CPU.h"

// operations:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// LDA:

void operation_LDA_Immediate(CPU *cpu, Memory *mem)
{
    byte imm = mem->data[cpu->PC];
    cpu->A = imm;
    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    cpu->PC++;
    CPU_tick(cpu, 2);
}

void operation_LDA_Absolute(CPU *cpu, Memory *mem)
{
    word abs = 0;
    abs += mem->data[cpu->PC];
    cpu->PC++;
    abs += mem->data[cpu->PC] * 0x100;
    cpu->A = mem->data[abs];

    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    cpu->PC++;
    CPU_tick(cpu, 4);
}

void operation_LDA_Absolute_X(CPU *cpu, Memory *mem)
{
    
}

void operation_LDA_Absolute_Y(CPU *cpu, Memory *mem)
{
}

void operation_LDA_Zero_Page(CPU *cpu, Memory *mem)
{
}

void operation_LDA_Zero_Page_X(CPU *cpu, Memory *mem)
{
}

void operation_LDA_indirect_X(CPU *cpu, Memory *mem)
{
}

void operation_LDA_indirect_Y(CPU *cpu, Memory *mem)
{
}
