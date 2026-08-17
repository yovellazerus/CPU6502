
#include "common.h"

struct Pipe {
    char data[PIPE_SIZE];
    uint8_t  read_index;
    uint8_t  write_index;  
    uint8_t  read_open; 
    uint8_t  write_open; 
};

void pipe_init(void){

    // register the pipes as a devices
    devsw_table[DEVICE_MAJOR_PIPE].close = pipe_close;
    devsw_table[DEVICE_MAJOR_PIPE].open  = pipe_open;
    devsw_table[DEVICE_MAJOR_PIPE].read  = pipe_read;
    devsw_table[DEVICE_MAJOR_PIPE].write = pipe_write;
    devsw_table[DEVICE_MAJOR_PIPE].ioctl = pipe_ioctl;
    devsw_table[DEVICE_MAJOR_PIPE].stat  = pipe_stat;
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

int pipe_stat(File* f, Stat* st){
    (void)f; (void)st;
    panic("pipe_stat");
    return -1;
}