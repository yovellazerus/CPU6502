
#include "comman.h"

// static void disk_read_sector(uint8_t* buffer, uint16_t lba){
//     MMIO16(DISK_LBA) = lba;
//     MMIO8(DISK_CMD) = DISK_CMD_READ;
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     memcpy(buffer, (uint8_t*)DISK_BUF, DISK_SECTOR_SIZE);
// }

// static void disk_write_sector(uint8_t* buffer, uint16_t lba){
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     MMIO16(DISK_LBA) = lba;
//     MMIO8(DISK_CMD) = DISK_CMD_WRITE;
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     memcpy((uint8_t*)DISK_BUF, buffer , DISK_SECTOR_SIZE);
// }

/* ========= shell ========================================================== */

#define MAX_LINE_SIZE 256
#define MAX_ARG_SIZE  32
#define MAX_ARGS      (MAX_LINE_SIZE / MAX_ARG_SIZE) 

typedef int (*Builtin)(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]);

int builtin_exit(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]);
int builtin_clear(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]);
int builtin_help(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]);
int builtin_write(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]);
int builtin_read(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]);

typedef struct {
    char*   name;
    Builtin builtin;
    char*   help;
} Prog;

Prog progs[] = {
    {"help",   builtin_help,   "Display this help message."},
    {"clear",  builtin_clear,  "Clear the terminal screen."},
    {"exit",   builtin_exit,   "Exit the simulator."},
    {"write",  builtin_write,  "Writes a message to the disk in a specific sector."},
    {"read",   builtin_read,   "Read a message in a specific sector from the disk."},
};

int builtin_write(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){
    (void)argc;
    (void)argv;
    TODO();
    return 0;
}

int builtin_read(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){
    (void)argc;
    (void)argv;
    TODO();
    return 0;
}

int builtin_exit(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){
    (void)argc;
    (void)argv;
    TODO();
    return 0;
}

int builtin_clear(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){
    (void)argc;
    (void)argv;
    TODO();
    return 0;
}

int builtin_help(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){
    (void)argc;
    (void)argv;
    TODO();
    return 0;
}

int execute_line(int argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){

    int i;
    // char* prg = argv[0];
    
    // if(argc == 0) return -1;
    
    // for(i = 0; i < ARRAY_SIZE(progs); i++){
    //     if(strcmp(prg, progs[i].name) == 0){
    //         return progs[i].builtin(argc, argv);
    //     }
    // }
    
    // printf("Unknown command: \"%s\".\n", prg);
    // return 1;
    
    // debug
    for(i = 0; i < argc; i++){
        printf("[%d] = \"%s\"\n", i, argv[i]);
    }
    return 0;
}

void parse_line(char* line, int* argc, char argv[MAX_ARGS][MAX_ARG_SIZE]){
    int i;
    *argc = 0;
    while(*line){
        
        // trim
        while(*line && (*line == ' ' || *line == '\n' || *line == '\t' || *line == '\v')){
            line++;
        }
        if(*line == '\0') break;
        
        // new argument
        i = 0;
        while(*line && *line != ' ' && *line != '\n' && *line != '\t' && *line != '\v'){
            argv[*argc][i++] = *line;
            if(i > MAX_ARG_SIZE - 1) panic("argument is to large");
            line++;
        }
        argv[*argc][i++] = '\0';
        (*argc)++;
        if(*argc > MAX_ARGS) panic("to many arguments");
    }
}

// ======= main =====================================================================================

static char line[MAX_LINE_SIZE];
static char argv[MAX_ARGS][MAX_ARG_SIZE];
static int argc;

void main(void) {

    printf("kernel is booting...\n");

    printf(
        "\n"
        "============================================================\n"
        "             File System Simulator Shell\n"
        "============================================================\n"
        "\n"
        "Type help to list available commands.\n"
        "Type exit to quit.\n"
        "\n"
    );
    
    while(true){
        argc = 0;
        printf("> ");
        gets(line, MAX_LINE_SIZE);
        *strchr(line, '\r') = '\0'; 
        parse_line(line, &argc, argv);
        if(argc == 0) continue;
        (void)execute_line(argc, argv);
    }
}
