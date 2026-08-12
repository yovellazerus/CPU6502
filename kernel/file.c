
#include "common.h"

/*
core UNIX file abstraction implementation
*/

struct File {
    File_Type type;
    uint8_t refcount;
    Device_Major major;
    uint8_t  readable;
    uint8_t  writable;
    uint32_t offset;
    union {
        Inode* inode;       // FILE_TYPE_INODE and FILE_TYPE_DEVICE
        Pipe*  pipe;        // FILE_TYPE_PIPE
    };
    // ...
};

File global_file_table[MAX_GLOBAL_OPEN_FILES];

File* file_get_by_global_index(uint8_t index){
    if(index < ARRAY_SIZE(global_file_table)){
        return &global_file_table[index];
    }
    return NULL;
}

bool file_open_global(  uint8_t index,
                        File_Type type,
                        Device_Major major,
                        uint8_t  readable,
                        uint8_t  writable,
                        uint32_t offset)
{
    File* file = file_get_by_global_index(index);
    if(!file) return false;
    file->type     = type;
    file->major    = major;
    file->readable = readable;
    file->writable = writable;
    file->offset   = offset;
    return true;
}                

File_Operations devsw_table[MAX_REGISTER_DEVICES];

bool register_device(Device_Major major, File_Operations* devops){
    if(major >= ARRAY_SIZE(devsw_table)){
        return false;
    }
    memcpy(&devsw_table[major], devops, sizeof(File_Operations));
    return true;
}

void file_init(void){

    // ...
}

/*
Unified system call interface for devices and regular files:
*/

int sys_read(void){
    Syscall_Argument arg;
    File* file;
    uint16_t user_ptr;
    int chunk_size;
    int bytes_read;
    uint8_t chunk_buffer[128]; // C stack allocted buffer
    int total_bytes_read = 0;

    if(!syscall_populate_argument(&arg)){
        return -1;
    }
    // validate fd
    if(arg.read.fd < 0 || arg.read.fd >= MAX_FILES_PER_PROC){
        return -1;
    }
    // validate permissions
    file = proc_get_file(current_process, arg.read.fd);
    if(!file || !file->readable){
        return -1;
    }

    /*
    loop for all the size requseted by the user, 
    and devid it to chunks (sizeof the buffer) 
    and dispatch them to the specific driver. 
    */
    user_ptr = (uint16_t)arg.read.buffer;
    while(arg.read.size > 0){
        // calculate chunk size
        chunk_size = (arg.read.size < sizeof(chunk_buffer)) ? arg.read.size : sizeof(chunk_buffer);

        // dispatch to driver using the devsw (fill the stack buffer)
        bytes_read = devsw_table[file->major].read(file, chunk_buffer, chunk_size);
        
        // handle read errors or EOF
        if(bytes_read < 0){
            if(total_bytes_read == 0) return -1;
            break;
        }
        if(bytes_read == 0){
            break; // EOF reached
        }
        
        // copy the data from the stack buffer TO the user's buffer
        if(copy_to_user(chunk_buffer, user_ptr, bytes_read, current_process) < 0){
            if(total_bytes_read == 0) return -1;
            break;
        } 

        // update variables
        total_bytes_read += bytes_read;
        user_ptr         += bytes_read;
        arg.read.size    -= bytes_read;

        // if the driver returned fewer bytes than requested, stop looping
        if(bytes_read < chunk_size){
            break; 
        }
    }

    return total_bytes_read;
}

int sys_write(void){
    Syscall_Argument arg;
    File* file;
    int chunk_size;
    uint16_t user_ptr;
    int bytes_written;
    uint8_t chunk_buffer[128]; // C stack allocated buffer
    int total_bytes_written = 0;

    if(!syscall_populate_argument(&arg)){
        return -1;
    }
    // validate fd
    if(arg.write.fd < 0 || arg.write.fd >= MAX_FILES_PER_PROC){
        return -1;
    }
    // validate permissions
    file = proc_get_file(current_process, arg.write.fd);
    if(!file || !file->writable){
        return -1;
    }

    /*
    loop for all the size requseted by the user, 
    and devid it to chunks (sizeof the buffer) 
    and dispatch them to the specific driver. 
    */
    user_ptr = (uint16_t)arg.write.buffer;
    while(arg.write.size > 0){
        // calculate chunk size
        chunk_size = (arg.write.size < sizeof(chunk_buffer)) ? arg.write.size : sizeof(chunk_buffer);

        // copy the data FROM the user's buffer to the stack buffer
        if(copy_from_user(chunk_buffer, user_ptr, chunk_size, current_process) < 0){
            if(total_bytes_written == 0) return -1;
            break;
        }

        // dispatch to driver using the devsw (write the data from the stack buffer)
        bytes_written = devsw_table[file->major].write(file, chunk_buffer, chunk_size);
        
        // handle write errors
        if(bytes_written < 0){
            if(total_bytes_written == 0) return -1;
            break;
        }

        // update variables
        total_bytes_written += bytes_written;
        user_ptr            += bytes_written;
        arg.write.size      -= bytes_written;

        // if the driver couldn't write the full chunk, stop looping
        if(bytes_written < chunk_size){
            break; 
        }
    }

    return total_bytes_written;
}

int sys_close(void){
    panic("sys_close");
    return 0;
}

int sys_open(void){
    panic("sys_open");
    return 0;
}

