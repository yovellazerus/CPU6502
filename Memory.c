
#include "Memory.h"

void Memory_init(Memory* memory){

    // for now ROM and RAM are the same memory component, this is not true in actual implementation...
    // this is ROM:
    memory->data[RESET_VECTOR_HIGH_ADDER] = HIGH_BYTE(ENTRY_POINT_ADDERS);
    memory->data[RESET_VECTOR_LOW_ADDER] = LOW_BYTE(ENTRY_POINT_ADDERS);

    memory->data[IRQ_VECTOR_HIGH_ADDER] = HIGH_BYTE(IRQ_HANDLER_ADDRES);
    memory->data[IRQ_VECTOR_LOW_ADDER] = LOW_BYTE(IRQ_HANDLER_ADDRES);

    memory->data[NMI_VECTOR_HIGH_ADDER] = HIGH_BYTE(NMI_HANDLER_ADDRES);
    memory->data[NMI_VECTOR_LOW_ADDER] = LOW_BYTE(NMI_HANDLER_ADDRES);

    // default NMI AND BRK/IRQ handler functions only return from the interrupt (opcode 0x40)
    // this is the OS for now :-) 
    memory->data[IRQ_HANDLER_ADDRES] = 0x40;
    memory->data[NMI_HANDLER_ADDRES] = 0x40;

    // debug:
    for(int i = 0; i < MEMORY_SIZE; i++){
        memory->label_table[i] = NULL;
    }
}

// TODO: weird bug, not printing labels not in new line 
void Memory_dump_all(Memory* memory, FILE* stream){
    word line = 0;
    bool toPrintLabel = false;
    for(int i = 0; i < MEMORY_SIZE; i++){
        toPrintLabel = false;
        if(memory->label_table[i] != NULL){
            toPrintLabel = true;
        }
        if(i % 8 == 0){
            if(toPrintLabel) fprintf(stream, "  %s <0x%.4x>", memory->label_table[i], i);
            fprintf(stream, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(stream, " 0x%.2x ", memory->data[i]);
    }
}

void Memory_dump_stack(Memory *memory, byte sp, FILE *stream)
{
    for(int i = STACK_END; i >= STACK_START; i--){
        fprintf(stream, "0x%.4x: ", i);
        fprintf(stream, " 0x%.2x", memory->data[i]);
        if(i == sp + STACK_START){
            fprintf(stream, " <--- sp\n");
        }
        else{
            fprintf(stream, "\n");
        }
    }
}

void Memory_load_code(Memory* memory, const char* label, word entry_point, byte* code, size_t size){
    word start = entry_point;
    for(size_t curr = 0; curr < size; curr++){
        memory->data[start + curr] = code[curr];
    }
    memory->label_table[start] = label;
}
