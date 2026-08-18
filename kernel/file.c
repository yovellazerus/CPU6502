
#include "common.h"

/*
core UNIX file abstraction implementation
unified system call interface for devices and regular files
*/

File global_file_table[MAX_GLOBAL_OPEN_FILES];

File_Operations* devsw_table[MAX_REGISTER_DEVICES];

void file_init(void){
    // ...
}

/*
syscalls using the devsw and the File_Operations struct for driver dispatch:
*/

int sys_read(void){
    Syscall_Argument arg;
    File* file;

    if(!syscall_populate_argument(&arg)) return -1;
    
    // validate fd
    if(arg.read.fd < 0 || arg.read.fd >= MAX_FILES_PER_PROC) return -1;
    
    // validate permissions
    file = proc_get_file(current_process, arg.read.fd);
    if(!file || !file->readable) return -1;

    //dispatch to the driver, it is responsible for safely getting data into the user buffer
    return file->fops->read(file, arg.read.buffer, arg.read.size);
}

int sys_write(void){
    Syscall_Argument arg;
    File* file;

    if(!syscall_populate_argument(&arg)) return -1;
    
    // validate fd
    if(arg.write.fd < 0 || arg.write.fd >= MAX_FILES_PER_PROC) return -1;
    
    // validate permissions
    file = proc_get_file(current_process, arg.write.fd);
    if(!file || !file->writable) return -1;

    // dispatch to the driver, pass the user virtual address
    return file->fops->write(file, arg.write.buffer, arg.write.size);
}

int sys_close(void){
    panic("sys_close");
    return -1;
}

int sys_open(void){
    panic("sys_open");
    return -1;
}

int sys_ioctl(void){
    panic("sys_ioctl");
    return -1;
}

/*
file related syscalls, with no underlining driver: (no File_Operations involved)
*/

int sys_fstat(void){
    panic("sys_stat");
    return -1;
}

int sys_fseek(void){
    panic("sys_fseek");
    return -1;
}

