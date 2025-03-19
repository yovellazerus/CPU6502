
#include "CPU.h"

// operations:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

void operation_STA_Absolute(CPU* cpu, Memory* mem)
{
    word abs = 0;
    abs += mem->data[cpu->PC];
    cpu->PC++;
    abs += mem->data[cpu->PC] * 0x100;
    mem->data[abs] = cpu->A;

    cpu->PC++;
    CPU_tick(cpu, 4);
}

void operation_TAX_Implied(CPU* cpu, Memory* mem)
{
    cpu->X = cpu->A;
    if(cpu->X == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->X)){
        CPU_onFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2);
}

void operation_TAY_Implied(CPU* cpu, Memory* mem)
{
    cpu->Y = cpu->A;
    if(cpu->Y == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->Y)){
        CPU_onFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2); 
}

void operation_TXA_Implied(CPU* cpu, Memory* mem)
{
    cpu->A = cpu->X;
    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2); 
}

void operation_TYA_Implied(CPU* cpu, Memory* mem)
{
    cpu->A = cpu->Y;
    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2); 
}

void operation_PHA_Implied(CPU* cpu, Memory* mem)
{
    mem->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->A;
    cpu->SP--;
    CPU_tick(cpu, 3);
}

void operation_PLA_Implied(CPU* cpu, Memory* mem)
{
    cpu->SP++;
    cpu->A = mem->data[cpu->SP + STACK_HIGH_ADDRES];
    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    CPU_tick(cpu, 4);

}

void operation_NOP_Implied(CPU* cpu, Memory* mem)
{
    cpu->PC++;
    CPU_tick(cpu, 2);
}