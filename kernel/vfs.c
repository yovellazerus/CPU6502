
#include "comman.h"

/*
core UNIX file abstraction implementation
*/

struct File {
    VFile_Type type;
    Device_Major major;
    uint8_t  readable;
    uint8_t  writable;
    uint32_t offset;
    // ...
};

File global_file_table[MAX_GLOBAL_OPEN_FILES];

File* file_get_by_global_index(uint8_t index){
    if(index < ARRAY_SIZE(global_file_table)){
        return &global_file_table[index];
    }
    return NULL;
}

struct Device_Operation {
    int (*read)(File*, void*, uint16_t);
    int (*write)(File*, void*, uint16_t);
    int (*close)(File*);
};

#define MACK_DEVICE(major_number, read_fn, write_fn, close_fn)      \
    devsw_table[major_number].read = read_fn;                       \
    devsw_table[major_number].write = write_fn;                     \
    devsw_table[major_number].close = close_fn

Device_Operation devsw_table[256];

void vfs_init(void){

    memset(global_file_table, 0, sizeof(global_file_table));
    
    // manually configure the very first entry in the global file table to the console
    global_file_table[0].type = VFILE_TYPE_DEVICE;
    global_file_table[0].major = DEVICE_MAJOR_CONSOLE;
    global_file_table[0].readable = 1;
    global_file_table[0].writable = 1;
    global_file_table[0].offset = 0;
    // ...

    memset(devsw_table, 0, sizeof(devsw_table));
    
    MACK_DEVICE(DEVICE_MAJOR_CONSOLE, console_read, console_write, console_close);
    // ...
}

/*
Unified system call interface for devices and regular files:
*/

int sys_read(void){
    static char read_buffer[256];
    SyscallArg syscall_argument;
    File* file;
    int bytes_read;
    uint16_t ax = proc_get_ax(current_process);
    const uint8_t* page_table = proc_get_page_table(current_process);

    memset(read_buffer, 0, sizeof(read_buffer));

    // populate the system call argument
    if(ax >= proc_get_top(current_process) || copy_from_user(&syscall_argument, ax, sizeof(syscall_argument), page_table) < 0){
        printk("\tsyscall argument pointer out of user space!\n");
        return -1;
    }

    if(syscall_argument.read.size >= sizeof(read_buffer)){
        printk("\tread is limited to %d bytes for now", sizeof(read_buffer));
        return -1;
    }

    if(syscall_argument.read.fd < 0 || syscall_argument.read.fd >= MAX_FILES_PER_PROC){
        printk("\tfd %d is not valid", syscall_argument.read.fd);
        return -1;
    }
    
    file = proc_get_file(current_process, syscall_argument.read.fd);
    if(!file || !file->readable){
        printk("\tfd %d is not open for reading", syscall_argument.read.fd);
        return -1;
    }

    // dispatch using the devsw
    bytes_read = devsw_table[file->major].read(file, read_buffer, syscall_argument.read.size);
    
    // copy the data from the kernel read buffer to the user 
    if(copy_to_user(read_buffer, (uint16_t)syscall_argument.read.buffer, bytes_read, (uint8_t*)page_table) < 0){
        printk("\tfailed to copy data to user space");
        return -1;
    }

    file->offset += bytes_read;
    return bytes_read;
}

int sys_write(void){
    static char write_buffer[256];
    SyscallArg syscall_argument;
    File* file;
    int bytes_written;
    uint16_t ax = proc_get_ax(current_process);
    const uint8_t* page_table = proc_get_page_table(current_process);

    memset(write_buffer, 0, sizeof(write_buffer));

    // 1. Populate the system call argument struct
    if(ax >= proc_get_top(current_process) || copy_from_user(&syscall_argument, ax, sizeof(syscall_argument), page_table) < 0){
        printk("\tsyscall argument pointer out of user space!\n");
        return -1;
    }

    if(syscall_argument.write.size >= sizeof(write_buffer)){
        printk("\twrite is limited to %d bytes for now", sizeof(write_buffer));
        return -1;
    }

    if(syscall_argument.write.fd < 0 || syscall_argument.write.fd >= MAX_FILES_PER_PROC){
        printk("\tfd %d is not valid", syscall_argument.write.fd);
        return -1;
    }
    
    file = proc_get_file(current_process, syscall_argument.write.fd);
    if(!file || !file->writable){
        printk("\tfd %d is not open for writing", syscall_argument.write.fd);
        return -1;
    }

    // copy the data form the user's buffer to the kernel's write buffer
    if(copy_from_user(write_buffer, (uint16_t)syscall_argument.write.buffer, syscall_argument.write.size, page_table) < 0){
        printk("\tuser pointer out of user space");
        return -1;
    }

    // dispatch useing the devsw
    bytes_written = devsw_table[file->major].write(file, write_buffer, syscall_argument.write.size);
    
    file->offset += bytes_written;
    return bytes_written;
}

int sys_close(void){
    panic("sys_close");
    return 0;
}

int sys_open(void){
    panic("sys_open");
    return 0;
}

