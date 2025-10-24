
#include "../include/cpu.h"
#include "../include/instruction.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, COLOR_RED "USAGE: %s <memory_img.bin>\n", argv[0]);
        fprintf(stderr, "ERROR: no input was provided.\n" COLOR_RESET);
        return 1;
    }
    if(argc > 2){
        fprintf(stderr, COLOR_RED "USAGE: %s <memory_img.bin>\n", argv[0]);
        fprintf(stderr, "ERROR: to many arguments.\n" COLOR_RESET);
        return 1;
    }

    CPU cpu;
    byte disk[DISK_BLOCK_COUNT][BLOCK_SIZE];

    const char* memory_path = argv[1];
    FILE* memory_file = fopen(argv[1], "rb");
    if(!memory_file){
        perror(memory_path);
        return 1;
    }
    fread(cpu.memory, sizeof(*cpu.memory), MEMORY_SIZE, memory_file);
    fclose(memory_file);

    const char* disk_path = "./bin/disk.bin";
    FILE* disk_file = fopen(disk_path, "rb+");
    if(!disk_file){
        perror(disk_path);
        return 1;
    }
    fread(disk, sizeof(**disk), BLOCK_SIZE*DISK_BLOCK_COUNT, disk_file);

    printf(COLOR_GREEN);
    CPU_reset(&cpu);
    CPU_run(&cpu, disk);
    printf(COLOR_RESET);

    fwrite(disk, sizeof(**disk), BLOCK_SIZE*DISK_BLOCK_COUNT, disk_file);
    fclose(disk_file);

    const char* memory_dump_path = "./bin/memory_dump.bin";
    FILE* memory_dump_file = fopen(memory_dump_path, "wb");
    if(!memory_dump_file){
        perror(memory_dump_path);
        return 1;
    }
    fwrite(cpu.memory, sizeof(*cpu.memory), MEMORY_SIZE, memory_dump_file);
    fclose(memory_dump_file);

    return 0;
}
