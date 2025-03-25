
#include "Memory.h"

void Memory_init(Memory* mem){
    for(int i = 0; i < MEM_SIZE; i++){
        mem->data[i] = 0;
    }
    mem->data[RESET_VECTOR_HIGH_BYTE] = PROGRAM_ENTRY_HIGH_BYTE;
    mem->data[RESET_VECTOR_LOW_BYTE] = PROGRAM_ENTRY_LOW_BYTE;

    mem->data[INTERRUPT_VECTOR_HIGH_BYTE] = INTERRUPT_HANDLER_HIGH_BYTE;
    mem->data[INTERRUPT_VECTOR_LOW_BYTE] = INTERRUPT_HANDLER_LOW_BYTE;

    mem->data[NMI_VECTOR_HIGH_BYTE] = NMI_HANDLER_HIGH_BYTE;
    mem->data[NMI_VECTOR_LOW_BYTE] = NMI_HANDLER_LOW_BYTE;

    // default NMI AND BRK/IRQ handler functions only return from the interrupt (opcode 0x40) 
    mem->data[INTERRUPT_HANDLER_HIGH_BYTE * 0x0100 + INTERRUPT_HANDLER_LOW_BYTE] = 0x40;
    mem->data[NMI_HANDLER_HIGH_BYTE * 0x0100 + NMI_HANDLER_LOW_BYTE] = 0x40;
}

void Memory_dump_all(Memory* mem, FILE* stream){
    word line = 0;
    for(int i = 0; i < MEM_SIZE; i++){
        if(i % 8 == 0){
            fprintf(stream, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(stream, " 0x%.2x ", mem->data[i]);
    }
}

void Memory_dump_stack(Memory *mem, byte sp,FILE *stream)
{
    for(int i = STACK_HIGH_ADDRES + SP_HIGH_VALUE; i >= STACK_HIGH_ADDRES; i--){
        fprintf(stream, "0x%.4x: ", i);
        fprintf(stream, " 0x%.2x", mem->data[i]);
        if(i == sp + STACK_HIGH_ADDRES){
            fprintf(stream, " <--- sp\n");
        }
        else{
            fprintf(stream, "\n");
        }
    }
}
