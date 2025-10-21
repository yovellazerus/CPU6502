
#include "../include/cpu.h"
#include "../include/instruction.h"

int main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(stderr, COLOR_RED "USAGE: %s <memory.bin> <disk.bin>\n", argv[0]);
        fprintf(stderr, "ERROR: no input was provided\n" COLOR_RESET);
        return 1;
    }

    CPU cpu;
    byte disk[DISK_BLOCK_COUNT][BLOCK_SIZE];

    const char* memory_path = argv[1];
    FILE* memory_img = fopen(argv[1], "rb");
    if(!memory_img){
        perror(memory_path);
        return 1;
    }
    fread(cpu.memory, sizeof(*cpu.memory), MEMORY_SIZE, memory_img);
    fclose(memory_img);

    const char* disk_path = argv[2];
    FILE* disk_img = fopen(disk_path, "rwb");
    if(!disk_img){
        fclose(memory_img);
        perror(disk_path);
        return 1;
    }
    fread(disk, sizeof(**disk), BLOCK_SIZE*DISK_BLOCK_COUNT, disk_img);

    CPU_reset(&cpu);
    printf(COLOR_GREEN);
    CPU_run(&cpu, disk);
    printf(COLOR_RESET);

    fwrite(disk, sizeof(**disk), BLOCK_SIZE*DISK_BLOCK_COUNT, disk_img);
    fclose(disk_img);

    const char* memory_dump_path = "./output/memory.bin";
    FILE* memory_dump_file = fopen(memory_dump_path, "wb");
    if(!memory_dump_file){
        perror(memory_dump_path);
        return 1;
    }
    fwrite(cpu.memory, sizeof(*cpu.memory), MEMORY_SIZE, memory_dump_file);
    fclose(memory_dump_file);

    return 0;
}
