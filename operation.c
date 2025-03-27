
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
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
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
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
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
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->X)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2);
}

void operation_TAY_Implied(CPU* cpu, Memory* mem)
{
    cpu->Y = cpu->A;
    if(cpu->Y == 0){
        CPU_onFlag(cpu, 'z');
    }
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->Y)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2); 
}

void operation_TXA_Implied(CPU* cpu, Memory* mem)
{
    cpu->A = cpu->X;
    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
    }
    CPU_tick(cpu, 2); 
}

void operation_TYA_Implied(CPU* cpu, Memory* mem)
{
    cpu->A = cpu->Y;
    if(cpu->A == 0){
        CPU_onFlag(cpu, 'z');
    }
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
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
    else{
        CPU_offFlag(cpu, 'z');
    }
    if(IS_SIGN_BYTE(cpu->A)){
        CPU_onFlag(cpu, 'n');
    }
    else{
        CPU_offFlag(cpu, 'n');
    }
    CPU_tick(cpu, 4);

}

void operation_JMP_Absolute(CPU *cpu, Memory *mem)
{
    word abs = 0;
    abs += mem->data[cpu->PC];
    cpu->PC++;
    abs += mem->data[cpu->PC] * 0x100;
    cpu->PC = abs;

    CPU_tick(cpu, 3);
}

void operation_JMP_Indirect(CPU *cpu, Memory *mem)
{
    word abs = 0;
    abs += mem->data[cpu->PC];
    cpu->PC++;
    abs += mem->data[cpu->PC] * 0x100;

    word indr = 0;
    indr += mem->data[abs];
    /*
    NB:
    An original 6502 has does not correctly fetch the target address,
    if the indirect vector falls on a page boundary (e.g. $xxFF where xx is any value from $00 to $FF). 
    In this case fetches the LSB from $xxFF as expected but takes the MSB from $xx00. 
    This is fixed in some later chips like the 65SC02,
    so for compatibility always ensure the indirect vector is not at the end of the page.
    */
    if(indr != 0xff){
        indr += mem->data[abs + 1] * 0x100;
    }
    
    cpu->PC = mem->data[indr];

    CPU_tick(cpu, 5);
}

void operation_JSR_Absolute(CPU *cpu, Memory *mem)
{
    word addr = 0;
    addr += mem->data[cpu->PC];
    cpu->PC++;
    addr += mem->data[cpu->PC] * 0x0100;
    cpu->PC++;

    // TODO: not pushing (PC - 1) dow to CPU implementation, maybe need to modify...
    mem->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC / 0x0100;
    cpu->SP--;
    mem->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC % 0x0100;
    cpu->SP--;

    cpu->PC = addr;

    CPU_tick(cpu, 6);
}

void operation_RTS_Implied(CPU *cpu, Memory *mem)
{
    word addr = 0;
    cpu->SP++;
    addr += mem->data[cpu->SP + STACK_HIGH_ADDRES];
    cpu->SP++;
    addr += mem->data[cpu->SP + STACK_HIGH_ADDRES] * 0x0100;
    
    cpu->PC = addr;
    CPU_tick(cpu, 6);
}

void operation_BEQ_Relative(CPU *cpu, Memory *mem)
{
    /*
    Branch instructions use relative address to identify the target instruction if they are executed.
    As relative addresses are stored using a signed 8 bit byte,
    the target instruction must be within *126*(TODO: Not sure I'm handling this correctly.) 
    bytes before the branch or 128 bytes after the branch.
    */
    signed char arg = (signed char)mem->data[cpu->PC];
    cpu->PC++;
    if(CPU_getFlag(cpu, 'z')){
        CPU_tick(cpu, 1);
        word old_cp = cpu->PC;
        word new_cp = cpu->PC + arg;
        if(old_cp & 0xff00 != new_cp & 0xff00){ // branching to a new page
            CPU_tick(cpu, 1);
        }
        cpu->PC = new_cp;
    }
    CPU_tick(cpu, 2);
}

void operation_BRK_Implied(CPU *cpu, Memory *mem)
{
    word addr = 0;
    addr += mem->data[INTERRUPT_VECTOR_LOW_BYTE];
    addr += mem->data[INTERRUPT_VECTOR_HIGH_BYTE] * 0x0100;

    // not pushing (PC - 1) dow to CPU implementation, maybe need to modify...
    mem->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC / 0x0100;
    cpu->SP--;
    mem->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC % 0x0100;
    cpu->SP--;

    // push P(flags on the stack)
    mem->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->P;
    cpu->SP--;

    CPU_onFlag(cpu, 'b');

    cpu->PC = addr;

    CPU_tick(cpu, 7);
}

void operation_NOP_Implied(CPU *cpu, Memory *mem)
{
    CPU_tick(cpu, 2);
}

void operation_RTI_Implied(CPU *cpu, Memory *mem)
{
    cpu->SP++;
    cpu->P = mem->data[cpu->SP + STACK_HIGH_ADDRES];

    word addr = 0;
    cpu->SP++;
    addr += mem->data[cpu->SP + STACK_HIGH_ADDRES];
    cpu->SP++;
    addr += mem->data[cpu->SP + STACK_HIGH_ADDRES] * 0x0100;

    cpu->PC = addr;

    CPU_tick(cpu, 6);
}
