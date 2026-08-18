
#include "common.h"

struct Pipe {
    char data[PIPE_SIZE];
    uint8_t  read_index;
    uint8_t  write_index;  
    uint8_t  read_open; 
    uint8_t  write_open; 
};

File_Operations pipe_ops = {
    pipe_open,
    pipe_close,
    pipe_read,
    pipe_write,
    pipe_ioctl,
};

void pipe_init(void){
    // register the pipes as devices
    devsw_table[DEVICE_MAJOR_PIPE] = &pipe_ops;
    // ...
}

int pipe_open(File* f){
    (void)f;
    panic("pipe_open");
    return -1;
}

int pipe_close(File* f){
    (void)f;
    panic("pipe_close");
    return -1;
}

int pipe_read(File* f, void* buffer, uint16_t length){
    (void)f; (void)buffer; (void)length;
    panic("pipe_read");
    return -1;
}

int pipe_write(File* f, void* buffer, uint16_t length){
    (void)f; (void)buffer; (void)length;
    panic("pipe_write");
    return -1;
}

int pipe_ioctl(File* f, uint8_t request, void* arg){
    (void)f; (void)request; (void)arg;
    panic("pipe_ioctl");
    return -1;
}
