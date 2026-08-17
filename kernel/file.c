
#include "common.h"

File global_file_table[MAX_GLOBAL_OPEN_FILES];

File_Operations devsw_table[MAX_REGISTER_DEVICES];

void file_init(void){
    // ...
}

/*
core UNIX file abstraction implementation
unified system call interface for devices and regular files
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

int sys_stat(void){
    panic("sys_stat");
    return -1;
}

