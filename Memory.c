
#include "Memory.h"

void Memory_set_entry_point(Memory* memory, word entry_point){
    memory->data[RESET_VECTOR_HIGH_BYTE] = (byte)(entry_point / 0x0100);
    memory->data[RESET_VECTOR_LOW_BYTE] = (byte)(entry_point % 0x0100);
}

void Memory_init(Memory* memory){
    // This needs to be done at the software level!
    // And actually it's the operating system's responsibility, if necessary...
    // for(int i = 0; i < MEMORY_SIZE; i++){
    //     memory->data[i] = 0x00;
    // }
    
    // for now ROM and RAM are the same memory component, this is not true in actual implementation...
    // this is ROM:
    memory->data[RESET_VECTOR_HIGH_BYTE] = GLOBAL_PROGRAM_ENTRY_LOW_BYTE;
    memory->data[RESET_VECTOR_LOW_BYTE] = GLOBAL_PROGRAM_ENTRY_HIGH_BYTE;

    memory->data[INTERRUPT_VECTOR_HIGH_BYTE] = INTERRUPT_HANDLER_HIGH_BYTE;
    memory->data[INTERRUPT_VECTOR_LOW_BYTE] = INTERRUPT_HANDLER_LOW_BYTE;

    memory->data[NMI_VECTOR_HIGH_BYTE] = NMI_HANDLER_HIGH_BYTE;
    memory->data[NMI_VECTOR_LOW_BYTE] = NMI_HANDLER_LOW_BYTE;

    // default NMI AND BRK/IRQ handler functions only return from the interrupt (opcode 0x40)
    // this is the OS for now :-) 
    memory->data[INTERRUPT_HANDLER_HIGH_BYTE * 0x0100 + INTERRUPT_HANDLER_LOW_BYTE] = 0x40;
    memory->data[NMI_HANDLER_HIGH_BYTE * 0x0100 + NMI_HANDLER_LOW_BYTE] = 0x40;

    // debug:
    for(int i = 0; i < MEMORY_SIZE; i++){
        memory->label_table[i] = NULL;
    }
}

void Memory_dump_all(Memory* memory, FILE* stream){
    word line = 0;
    for(int i = 0; i < MEMORY_SIZE; i++){
        if(i % 8 == 0){
            if(memory->label_table[line]) fprintf(stream, "  %s <0x%.4x>", memory->label_table[line], line);
            fprintf(stream, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(stream, " 0x%.2x ", memory->data[i]);
    }
}

void Memory_dump_stack(Memory *memory, byte sp,FILE *stream)
{
    for(int i = STACK_HIGH_ADDRES + SP_HIGH_VALUE; i >= STACK_HIGH_ADDRES; i--){
        fprintf(stream, "0x%.4x: ", i);
        fprintf(stream, " 0x%.2x", memory->data[i]);
        if(i == sp + STACK_HIGH_ADDRES){
            fprintf(stream, " <--- sp\n");
        }
        else{
            fprintf(stream, "\n");
        }
    }
}

void Memory_program_init(Program* program, 
                        const char* label, 
                        word entry_point, 
                        byte* code, 
                        size_t size){

    program->label = label;
    program->entry_point = entry_point;
    program->code = code;
    program->size = size;
}

void Memory_load_program(Memory* memory, Program* program){
    word start = program->entry_point;
    for(size_t curr = 0; curr < program->size; curr++){
        memory->data[start + curr] = program->code[curr];
    }
    memory->label_table[start] = program->label;
}

void Memory_load_code(Memory* memory, const char* label, word entry_point, byte* code, size_t size){
    word start = entry_point;
    for(size_t curr = 0; curr < size; curr++){
        memory->data[start + curr] = code[curr];
    }
    memory->label_table[start] = label;
}
