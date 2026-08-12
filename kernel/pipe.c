
#include "common.h"

struct Pipe {
    char data[PIPE_SIZE];
    uint16_t read_number;  // number of bytes read
    uint16_t write_number; // number of bytes written
    uint8_t  read_open;    // read fd is still open
    uint8_t  write_open;   // write fd is still open
};

// TODO: pipe functions...