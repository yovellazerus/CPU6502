
#include "common.h"

struct Pipe {
    char data[PIPE_SIZE];
    uint16_t read_index;
    uint16_t write_index;  
    uint8_t  read_open; 
    uint8_t  write_open; 
};

// TODO: pipe functions...