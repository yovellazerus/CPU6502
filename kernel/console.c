
#include "comman.h"

void console_init(void){
    Device_Ops console_devops;
    console_devops.close = console_close;
    console_devops.read  = console_read;
    console_devops.write = console_write;
    // manually open the very first entry in the global file table to the console
    if(!file_open_global(   0,
                            VFILE_TYPE_DEVICE,
                            DEVICE_MAJOR_CONSOLE,
                            1,
                            1,
                            0
                        )
                                                )
    {
        goto bad;
    }
    if(!register_device(DEVICE_MAJOR_CONSOLE, &console_devops)){
        goto bad;
    }
    return;
bad:
    panic("console_init");
}

// using cooked mode
int console_read(File* file, void* dst, uint16_t n){
    int c;
    char* dest = (char*)dst;
    int bytes_read = 0;
    (void)file;

    // leave 1 byte for the '\n'
    while(bytes_read < n - 1){

        // modifies a the uart ring buffer that is accessible to interrupts therefore, must be locked
        INTER_OFF();
        c = uart_getc();
        if(c == -1){
            // ring buffer is empty
            sleep(&ring_buffer);
            INTER_ON();
            // very importent! if 2 processes will wakeup at the same time,
            // the first will read the character, and the second will get -1 form uart_getc()
            // and will go back to sleep
            continue;
        }
        INTER_ON();

        // BACKSPACE or DEL
        if(c == '\b' || c == 0x7f){
            if(bytes_read > 0){
                // remove from user buffer
                bytes_read--;

                // deleting the character visually
                uart_putc_sync('\b'); 
                uart_putc_sync(' ');
                uart_putc_sync('\b');
            }
            // skip adding the BACKSPACE to the ring buffer
            continue;
        }

        // TODO: need to check for '\0'?

        // is it the end of the line?
        if(c == '\r' || c == '\n'){
            // standard UNIX '\n' in the user buffer
            dest[bytes_read++] = '\n';

            uart_putc_sync('\r');
            uart_putc_sync('\n');
            break;
        }

        // normal character
        dest[bytes_read++] = c;
        // echo
        uart_putc_sync(c);
    }

    return bytes_read;
}

int console_write(File* file, void* src, uint16_t n){
    uint16_t bytes_written;
    char* source = (char*)src;
    (void)file;
    for(bytes_written = 0; bytes_written < n; bytes_written++){
        if(source[bytes_written] == '\n'){
            uart_putc('\r');
        }
        uart_putc(source[bytes_written]);
    }
    return bytes_written;
}

int console_close(File* file){
    (void)file;
    panic("console_close");
    return 0;
}
