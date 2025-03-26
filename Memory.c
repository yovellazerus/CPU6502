
#include "Memory.h"

void Memory_set_entry_point(Memory* mem, word entry_point){
    mem->data[RESET_VECTOR_HIGH_BYTE] = (byte)(entry_point / 0x0100);
    mem->data[RESET_VECTOR_LOW_BYTE] = (byte)(entry_point % 0x0100);
}

void Memory_init(Memory* mem, word entry_point){
    for(int i = 0; i < MEMORY_SIZE; i++){
        mem->data[i] = 0;
    }

    Memory_set_entry_point(mem, entry_point);
    
    mem->data[INTERRUPT_VECTOR_HIGH_BYTE] = INTERRUPT_HANDLER_HIGH_BYTE;
    mem->data[INTERRUPT_VECTOR_LOW_BYTE] = INTERRUPT_HANDLER_LOW_BYTE;

    mem->data[NMI_VECTOR_HIGH_BYTE] = NMI_HANDLER_HIGH_BYTE;
    mem->data[NMI_VECTOR_LOW_BYTE] = NMI_HANDLER_LOW_BYTE;

    // default NMI AND BRK/IRQ handler functions only return from the interrupt (opcode 0x40) 
    mem->data[INTERRUPT_HANDLER_HIGH_BYTE * 0x0100 + INTERRUPT_HANDLER_LOW_BYTE] = 0x40;
    mem->data[NMI_HANDLER_HIGH_BYTE * 0x0100 + NMI_HANDLER_LOW_BYTE] = 0x40;

    for(int i = 0; i < MEMORY_SIZE; i++){
        mem->label_table[i] = NULL;
    }
}

void Memory_dump_all(Memory* mem, FILE* stream){
    word line = 0;
    for(int i = 0; i < MEMORY_SIZE; i++){
        if(i % 8 == 0){
            if(mem->label_table[line]) fprintf(stream, "  %s <0x%.4x>", mem->label_table[line], line);
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

void Memory_load_program(Memory* mem, Program* program){
    word start = program->entry_point;
    for(size_t curr = 0; curr < program->size; curr++){
        mem->data[start + curr] = program->code[curr];
    }
    mem->label_table[start] = program->label;
}

void Memory_load_function(Memory* mem, const char* label, word entry_point, byte* code, size_t size){
    word start = entry_point;
    for(size_t curr = 0; curr < size; curr++){
        mem->data[start + curr] = code[curr];
    }
    mem->label_table[start] = label;
}
