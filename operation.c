
#include "CPU.h"

// auxiliary functions:

void assignment_flag_control(CPU *cpu, char reg){
    switch (reg)
    {
    case 'A':
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
        break;

    case 'X':
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
        break;

    case 'Y':
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
        break;
    
    default:
        assert(false && "unreachable");
        break;
    }
    
}

bool is_page_crossed(word addres, byte offset){
    word new_addres = addres + offset;
    if((addres & 0xff00) != (new_addres & 0xff00)){
        return true;
    }
    return false;
}


// operations:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void operation_LDA_Immediate(CPU *cpu, Memory *memory)
{
    byte imm = memory->data[cpu->PC];
    cpu->A = imm;
    cpu->PC++;

    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 2);
}

void operation_LDA_Absolute(CPU *cpu, Memory *memory)
{
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->A = memory->data[abs];

    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 4);
}

void operation_LDA_Absolute_X(CPU *cpu, Memory *memory)
{
    byte offset = cpu->X;
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->A = memory->data[abs + offset];

    assignment_flag_control(cpu, 'A');
    if(is_page_crossed(abs, offset)){
        CPU_tick(cpu, 1);
    }
    CPU_tick(cpu, 4);
}

void operation_LDA_Absolute_Y(CPU *cpu, Memory *memory)
{
    byte offset = cpu->Y;
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->A = memory->data[abs + offset];

    assignment_flag_control(cpu, 'A');
    if(is_page_crossed(abs, offset)){
        CPU_tick(cpu, 1);
    }
    CPU_tick(cpu, 4);
}

void operation_LDA_Zero_Page(CPU *cpu, Memory *memory)
{
    cpu->A = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 3);
}

void operation_LDA_Zero_Page_X(CPU *cpu, Memory *memory)
{
    cpu->A = memory->data[(ZERO_PAGE_START + memory->data[cpu->PC] + cpu->X) % PAGE_SIZE]; // zero page wrap around
    cpu->PC++;

    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 4);
}

void operation_LDA_Indirect_X(CPU *cpu, Memory *memory)
{
    word base_addres = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    base_addres += cpu->X % PAGE_SIZE; // zero page wrap around

    word real_addres = 0;
    real_addres += memory->data[ZERO_PAGE_START + base_addres];
    real_addres += memory->data[ZERO_PAGE_START + base_addres + 1] * 0x0100;

    cpu->A = memory->data[real_addres];

    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 6);
}

void operation_LDA_Indirect_Y(CPU *cpu, Memory *memory)
{
    word base_addres = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    word real_addres = 0;
    real_addres += memory->data[base_addres];
    real_addres += memory->data[base_addres + 1] * 0x0100;

    if(is_page_crossed(real_addres, cpu->Y)){
        CPU_tick(cpu, 1);
    }

    real_addres += cpu->Y;
    
    cpu->A = memory->data[real_addres];

    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 5);
}

void operation_LDX_Immediate(CPU* cpu, Memory* memory){
    byte imm = memory->data[cpu->PC];
    cpu->X = imm;
    cpu->PC++;

    assignment_flag_control(cpu, 'X');
    CPU_tick(cpu, 2);
}

void operation_LDX_Absolute(CPU* cpu, Memory* memory){
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->X = memory->data[abs];

    assignment_flag_control(cpu, 'X');
    CPU_tick(cpu, 4);
}

void operation_LDX_Absolute_Y(CPU* cpu, Memory* memory){
    byte offset = cpu->Y;
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->X = memory->data[abs + offset];

    assignment_flag_control(cpu, 'X');
    if(is_page_crossed(abs, offset)){
        CPU_tick(cpu, 1);
    }
    CPU_tick(cpu, 4);
}

void operation_LDX_Zero_Page(CPU* cpu, Memory* memory){
    cpu->X = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    assignment_flag_control(cpu, 'X');
    CPU_tick(cpu, 3);
}

void operation_LDX_Zero_Page_Y(CPU* cpu, Memory* memory){
    cpu->X = memory->data[(ZERO_PAGE_START + memory->data[cpu->PC] + cpu->Y) % PAGE_SIZE]; // zero page wrap around
    cpu->PC++;

    assignment_flag_control(cpu, 'X');
    CPU_tick(cpu, 4);
}

void operation_LDY_Immediate(CPU* cpu, Memory* memory){
    byte imm = memory->data[cpu->PC];
    cpu->Y = imm;
    cpu->PC++;

    assignment_flag_control(cpu, 'Y');
    CPU_tick(cpu, 2);
}

void operation_LDY_Absolute(CPU* cpu, Memory* memory){
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->Y = memory->data[abs];

    assignment_flag_control(cpu, 'Y');
    CPU_tick(cpu, 4);
}

void operation_LDY_Absolute_X(CPU* cpu, Memory* memory){
    byte offset = cpu->X;
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    cpu->Y = memory->data[abs + offset];

    assignment_flag_control(cpu, 'Y');
    if(is_page_crossed(abs, offset)){
        CPU_tick(cpu, 1);
    }
    CPU_tick(cpu, 4);
}

void operation_LDY_Zero_Page(CPU* cpu, Memory* memory){
    cpu->Y = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    assignment_flag_control(cpu, 'Y');
    CPU_tick(cpu, 3);
}

void operation_LDY_Zero_Page_X(CPU* cpu, Memory* memory){
    cpu->Y = memory->data[(ZERO_PAGE_START + memory->data[cpu->PC] + cpu->X) % PAGE_SIZE]; // zero page wrap around
    cpu->PC++;

    assignment_flag_control(cpu, 'Y');
    CPU_tick(cpu, 4);
}

void operation_STA_Absolute(CPU* cpu, Memory* memory)
{
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    memory->data[abs] = cpu->A;
    cpu->PC++;

    CPU_tick(cpu, 4);
}

void operation_STA_Zero_Page(CPU* cpu, Memory* memory){

    memory->data[ZERO_PAGE_START + memory->data[cpu->PC]] = cpu->A;
    cpu->PC++;

    CPU_tick(cpu, 3);
}

void operation_STA_Zero_Page_X(CPU* cpu, Memory* memory){

    memory->data[(ZERO_PAGE_START + memory->data[cpu->PC] + cpu->X) % PAGE_SIZE] = cpu->A; // zero page wrap around
    cpu->PC++;

    CPU_tick(cpu, 4);
}

void operation_STA_Absolute_X(CPU* cpu, Memory* memory){
    byte offset = cpu->X;
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    memory->data[abs + offset] = cpu->A;

    CPU_tick(cpu, 5);
}

void operation_STA_Absolute_Y(CPU* cpu, Memory* memory){
    byte offset = cpu->Y;
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC++;
    memory->data[abs + offset] = cpu->A;

    CPU_tick(cpu, 5);
}

void operation_STA_Indirect_X(CPU* cpu, Memory* memory){

    word base_addres = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    base_addres += cpu->X % PAGE_SIZE; // zero page wrap around

    word real_addres = 0;
    real_addres += memory->data[ZERO_PAGE_START + base_addres];
    real_addres += memory->data[ZERO_PAGE_START + base_addres + 1] * 0x0100;

    memory->data[real_addres] = cpu->A;

    CPU_tick(cpu, 6);
}

void operation_STA_Indirect_Y(CPU* cpu, Memory* memory){

    word base_addres = memory->data[ZERO_PAGE_START + memory->data[cpu->PC]];
    cpu->PC++;

    word real_addres = 0;
    real_addres += memory->data[base_addres];
    real_addres += memory->data[base_addres + 1] * 0x0100;

    real_addres += cpu->Y;
    
    memory->data[real_addres] = cpu->A;

    CPU_tick(cpu, 6);
}

void operation_STX_Zero_Page(CPU* cpu, Memory* memory){
    memory->data[ZERO_PAGE_START + memory->data[cpu->PC]] = cpu->X;
    cpu->PC++;

    CPU_tick(cpu, 3);
}

void operation_STX_Zero_Page_Y(CPU* cpu, Memory* memory){
    memory->data[(ZERO_PAGE_START + memory->data[cpu->PC] + cpu->Y) % PAGE_SIZE] = cpu->X; // zero page wrap around
    cpu->PC++;

    CPU_tick(cpu, 4);
}

void operation_STX_Absolute(CPU* cpu, Memory* memory){
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    memory->data[abs] = cpu->X;
    cpu->PC++;

    CPU_tick(cpu, 4);
}

void operation_STY_Zero_Page(CPU* cpu, Memory* memory){
    memory->data[ZERO_PAGE_START + memory->data[cpu->PC]] = cpu->Y;
    cpu->PC++;

    CPU_tick(cpu, 3);
}

void operation_STY_Zero_Page_X(CPU* cpu, Memory* memory){
    memory->data[(ZERO_PAGE_START + memory->data[cpu->PC] + cpu->X) % PAGE_SIZE] = cpu->Y; // zero page wrap around
    cpu->PC++;

    CPU_tick(cpu, 4);
}

void operation_STY_Absolute(CPU* cpu, Memory* memory){
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    memory->data[abs] = cpu->Y;
    cpu->PC++;

    CPU_tick(cpu, 4);
}

void operation_TAX_Implied(CPU* cpu, Memory* memory)
{
    cpu->X = cpu->A;
    assignment_flag_control(cpu, 'X');
    CPU_tick(cpu, 2);
}

void operation_TAY_Implied(CPU* cpu, Memory* memory)
{
    cpu->Y = cpu->A;
    assignment_flag_control(cpu, 'Y');
    CPU_tick(cpu, 2); 
}

void operation_TXA_Implied(CPU* cpu, Memory* memory)
{
    cpu->A = cpu->X;
    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 2); 
}

void operation_TYA_Implied(CPU* cpu, Memory* memory)
{
    cpu->A = cpu->Y;
    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 2); 
}

void operation_PHA_Implied(CPU* cpu, Memory* memory)
{
    memory->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->A;
    cpu->SP--;
    CPU_tick(cpu, 3);
}

void operation_PLA_Implied(CPU* cpu, Memory* memory)
{
    cpu->SP++;
    cpu->A = memory->data[cpu->SP + STACK_HIGH_ADDRES];
    assignment_flag_control(cpu, 'A');
    CPU_tick(cpu, 4);

}

void operation_JMP_Absolute(CPU *cpu, Memory *memory)
{
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;
    cpu->PC = abs;

    CPU_tick(cpu, 3);
}

void operation_JMP_Indirect(CPU *cpu, Memory *memory)
{
    word abs = 0;
    abs += memory->data[cpu->PC];
    cpu->PC++;
    abs += memory->data[cpu->PC] * 0x100;

    word indr = 0;
    indr += memory->data[abs];
    /*
    NB:
    An original 6502 has does not correctly fetch the target address,
    if the indirect vector falls on a page boundary (e.g. $xxFF where xx is any value from $00 to $FF). 
    In this case fetches the LSB from $xxFF as expected but takes the MSB from $xx00. 
    This is fixed in some later chips like the 65SC02,
    so for compatibility always ensure the indirect vector is not at the end of the page.
    */
    if(indr != 0xff){
        indr += memory->data[abs + 1] * 0x100;
    }
    
    cpu->PC = memory->data[indr];

    CPU_tick(cpu, 5);
}

void operation_JSR_Absolute(CPU *cpu, Memory *memory)
{
    word addr = 0;
    addr += memory->data[cpu->PC];
    cpu->PC++;
    addr += memory->data[cpu->PC] * 0x0100;
    cpu->PC++;

    // TODO: not pushing (PC - 1) dow to CPU implementation, maybe need to modify...
    memory->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC / 0x0100;
    cpu->SP--;
    memory->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC % 0x0100;
    cpu->SP--;

    cpu->PC = addr;

    CPU_tick(cpu, 6);
}

void operation_RTS_Implied(CPU *cpu, Memory *memory)
{
    word addr = 0;
    cpu->SP++;
    addr += memory->data[cpu->SP + STACK_HIGH_ADDRES];
    cpu->SP++;
    addr += memory->data[cpu->SP + STACK_HIGH_ADDRES] * 0x0100;
    
    cpu->PC = addr;
    CPU_tick(cpu, 6);
}

void operation_BEQ_Relative(CPU *cpu, Memory *memory)
{
    /*
    Branch instructions use relative address to identify the target instruction if they are executed.
    As relative addresses are stored using a signed 8 bit byte,
    the target instruction must be within *126*(TODO: Not sure I'm handling this correctly.) 
    bytes before the branch or 128 bytes after the branch.
    */
    signed char arg = (signed char)memory->data[cpu->PC];
    cpu->PC++;
    if(CPU_getFlag(cpu, 'z')){
        CPU_tick(cpu, 1);
        word old_cp = cpu->PC;
        word new_cp = cpu->PC + arg;
        if(is_page_crossed(old_cp, arg)){ // branching to a new page
            CPU_tick(cpu, 1);
        }
        cpu->PC = new_cp;
    }
    CPU_tick(cpu, 2);
}

void operation_CLC_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_offFlag(cpu, 'c'); }
void operation_CLD_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_offFlag(cpu, 'd'); }
void operation_CLI_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_offFlag(cpu, 'i'); }
void operation_CLV_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_offFlag(cpu, 'v'); }
void operation_SEC_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_onFlag(cpu, 'c');  }
void operation_SED_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_onFlag(cpu, 'd');  }
void operation_SEI_Implied(CPU* cpu, Memory* memory){ CPU_tick(cpu, 2); CPU_onFlag(cpu, 'i');  }

void operation_BRK_Implied(CPU *cpu, Memory *memory)
{
    word addr = 0;
    addr += memory->data[INTERRUPT_VECTOR_LOW_BYTE];
    addr += memory->data[INTERRUPT_VECTOR_HIGH_BYTE] * 0x0100;

    // not pushing (PC - 1) dow to CPU implementation, maybe need to modify...
    memory->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC / 0x0100;
    cpu->SP--;
    memory->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->PC % 0x0100;
    cpu->SP--;

    // push P(flags on the stack)
    memory->data[cpu->SP + STACK_HIGH_ADDRES] = cpu->P;
    cpu->SP--;

    cpu->PC = addr;

    CPU_onFlag(cpu, 'b');
    CPU_tick(cpu, 7);
}

void operation_NOP_Implied(CPU *cpu, Memory *memory)
{
    CPU_tick(cpu, 2);
}

void operation_RTI_Implied(CPU *cpu, Memory *memory)
{
    cpu->SP++;
    cpu->P = memory->data[cpu->SP + STACK_HIGH_ADDRES];

    word addr = 0;
    cpu->SP++;
    addr += memory->data[cpu->SP + STACK_HIGH_ADDRES];
    cpu->SP++;
    addr += memory->data[cpu->SP + STACK_HIGH_ADDRES] * 0x0100;

    cpu->PC = addr;

    CPU_tick(cpu, 6);
}
