
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <conio.h>

#include "MCS6502.h"
#include "ansi_codes.h"
#include "machine.h"

/* ======== MMIO devices ===========================================================*/

typedef MCS6502ExecutionContext CPU;

typedef struct {
    FILE* file;
    uint8_t buffer[DISK_SECTOR_SIZE];
    uint8_t cmd;
    uint8_t status;
    uint8_t lba_low;
    uint8_t lba_high;
    int delay;   // cycles remaining for operation
} Disk;

/* ======== the Machine itself ===========================================================*/

typedef struct {
      
    CPU* cpu;
    uint8_t* ram;
    uint8_t* rom;
    Uart* uart;
    Disk* disk;
    // ...

} Machine;

uint8_t Machine_read(uint16_t addr, void* ctx);
void    Machine_write(uint16_t addr, uint8_t byte, void* ctx);

Machine* Machine_create(const char* rom_path, const char* disk_path);
void     Machine_destroy(Machine* m);
void     Machine_coredump(const Machine* m, const char* path);
bool     Machine_step(Machine* m);

/* ======== main =============================================================*/

int main(int argc, char** argv)
{
    if(argc != 2){
        fprintf(stderr, COLOR_RED "USAGE: %s <disk.img>.\n" COLOR_RESET, argv[0]);
        return 1;
    }
    
    Machine* m = Machine_create("machine\\rom.bin", argv[1]);
    if(!m){
        fprintf(stderr, COLOR_RED "ERROR: failure to create the virtual machine form disk image: \"%s\".\n" COLOR_RESET, argv[1]);
        return 1;
    } 
    while(Machine_step(m));
    Machine_coredump(m, ".\\bin\\coredump.bin");
    Machine_destroy(m);

    return 0;
}

/* ======== read/write =======================================================*/

// TODO: MMU...
uint8_t Machine_read(uint16_t addr, void* ctx) {
    if(!ctx) return 0xFF;
    Machine* m = (Machine*)ctx;

    // ---------------- RAM ----------------
    if (addr < RAM_SIZE)
        return m->ram[addr];

    // ---------------- DISK buffer ----------------
    if (addr >= DISK_BUF && addr < DISK_BUF + sizeof(m->disk->buffer))
        return m->disk->buffer[addr - DISK_BUF];

    // ---------------- DISK registers ----------------
    if (addr == DISK_CMD)
        return m->disk->cmd;

    if (addr == DISK_STAT)
        return m->disk->status;

    if (addr == DISK_LBA + 0)
        return m->disk->lba_low;

    if (addr == DISK_LBA + 1)
        return m->disk->lba_high;

    // ---------------- UART ----------------
    if (addr == UART_RX)
    {
        uint8_t c = m->uart->rx;
        m->uart->status &= ~UART_STATUS_RX_READY;
        return c;
    }

    if (addr == UART_TX)
        return m->uart->tx;

    if (addr == UART_STAT)
        return m->uart->status;

    // ---------------- ROM ----------------
    if (addr >= ROM_BASE)
        return m->rom[addr - ROM_BASE];

    // ---------------- unmapped ----------------
    return 0xFF;
}

// TODO: MMU...
void Machine_write(uint16_t addr, uint8_t byte, void* ctx) {
    if(!ctx) return;
    Machine* m = (Machine*)ctx;

    // ---------------- RAM ----------------
    if (addr < RAM_SIZE)
    {
        m->ram[addr] = byte;
        return;
    }

    // ---------------- DISK buffer ----------------
    if (addr >= DISK_BUF && addr < DISK_BUF + sizeof(m->disk->buffer))
    {
        m->disk->buffer[addr - DISK_BUF] = byte;
        return;
    }

    // ---------------- DISK CMD ----------------
    if (addr == DISK_CMD)
    {
        m->disk->cmd = byte;

        if (byte == DISK_CMD_READ)
        {
            m->disk->status = DISK_STATUS_BUSY;
            m->disk->delay = DISK_LATENCY; 
        }

        else if (byte == DISK_CMD_WRITE)
        {
            m->disk->status = DISK_STATUS_BUSY;
            m->disk->delay = DISK_LATENCY; 
        }

        return;
    }
    
    // ---------------- DISK LBA ----------------
    if (addr == DISK_LBA + 0)
    {
        m->disk->lba_low = byte;
        return;
    }
    
    if (addr == DISK_LBA + 1)
    {
        m->disk->lba_high = byte;
        return;
    }

    // ---------------- UART TX ----------------
    if (addr == UART_TX)
    {
        m->uart->tx = byte;
        m->uart->status &= ~UART_STATUS_TX_READY;
        return;
    }
}

/* ======== functions ========================================================*/

Machine* Machine_create(const char* rom_path, const char* disk_path) {

    Machine* m = (Machine*)calloc(1, sizeof(Machine));
    if (!m) return NULL;

    m->cpu = (CPU*)calloc(1, sizeof(CPU));
    m->ram = (uint8_t*)calloc(RAM_SIZE, sizeof(uint8_t));
    m->rom = (uint8_t*)calloc(ROM_SIZE, sizeof(uint8_t));
    m->uart = (Uart*)calloc(1, sizeof(Uart));
    m->disk = (Disk*)calloc(1, sizeof(Disk));

    if (!m->ram || !m->rom || !m->uart || !m->disk || !m->cpu){
        Machine_destroy(m);
        return NULL;
    }
        
    // ROM
    FILE* rom_img = fopen(rom_path, "rb");
    if(!rom_img){
        Machine_destroy(m);
        return NULL;
    }
    ssize_t rom_size = fread(m->rom, 1, ROM_SIZE, rom_img);
    if(rom_size > ROM_SIZE){
        fclose(rom_img);
        Machine_destroy(m);
        return NULL; 
    }
    fclose(rom_img);
    
    // disk
    if(disk_path){
       m->disk->file = fopen(disk_path, "rb+");
       if(!m->disk->file){
           Machine_destroy(m);
           return NULL;
       }
       m->disk->status = ~DISK_STATUS_NONE; 
    }
    
    // CPU
    MCS6502Init(
        m->cpu,
        Machine_read,
        Machine_write,
        m
    );
    MCS6502Reset(m->cpu);
    
    // uart
    printf(COLOR_GREEN);

    return m;
}

void Machine_destroy(Machine* m) {
    if (!m) return;
    free(m->ram);
    free(m->rom);
    free(m->uart);
    if(m->disk->file) fclose(m->disk->file);
    free(m->disk);
    free(m->cpu);
    free(m);
    printf(COLOR_RESET);
}

void Machine_coredump(const Machine* m, const char* path) {
    if(!m || !path) return;
    
    FILE* f = fopen(path, "wb");
    if (!f) return;

    uint8_t mmio[256];
    memset(mmio, 0xff, sizeof(mmio));
    
    fwrite(m->ram, 1, RAM_SIZE, f);
    fwrite(m->disk->buffer, 1, DISK_SECTOR_SIZE, f);
    fwrite(mmio, 1, sizeof(mmio), f);
    fwrite(m->rom, 1, ROM_SIZE, f);
    
    fclose(f);
}

bool Machine_step(Machine* m){
    if(!m) return false;
    
    // keyboard
    if (_kbhit())
    {
        int c = _getch();
        
        // ESC 
        if(c == 0x1B){
            return false; // power-off
        }
        
        if (c != -1){
            m->uart->rx = (uint8_t)c;
            m->uart->status |= UART_STATUS_RX_READY;
        }
    }

    // display
    if (!(m->uart->status & UART_STATUS_TX_READY))
    {
        uint8_t byte = m->uart->tx;
        if(byte == '\r') byte = '\n';
        _putch(byte);
        m->uart->status |= UART_STATUS_TX_READY;
    }
    
    // disk
    if (m->disk->status == DISK_STATUS_BUSY)
    {
        if (--m->disk->delay == 0)
        {
            uint16_t lba = (m->disk->lba_high << 8) | m->disk->lba_low;

            if (lba > DISK_MAX_LBA)
            {
                m->disk->status = DISK_STATUS_ERROR;
            }
            else{
                if (m->disk->cmd = DISK_CMD_READ)
                {
                    fseek(m->disk->file, lba * DISK_SECTOR_SIZE, SEEK_SET);
                    fread(m->disk->buffer, 1, DISK_SECTOR_SIZE, m->disk->file);
                
                    m->disk->status = DISK_STATUS_READY;
                }
                else if (m->disk->cmd = DISK_CMD_WRITE)
                {
                    fseek(m->disk->file, lba * DISK_SECTOR_SIZE, SEEK_SET);
                    fwrite(m->disk->buffer, 1, DISK_SECTOR_SIZE, m->disk->file);
                
                    m->disk->status = DISK_STATUS_READY;
                }
                else{
                    m->disk->status = DISK_STATUS_ERROR;
                }
            }
        }
    }
    
    // CPU
    for(int i = 0; i < CPU_PER_STEP; i++){

        MCS6502ExecResult r = MCS6502ExecNext(m->cpu);
    
        // debug
        if (r == MCS6502ExecResultInvalidOperation)
        {
            fprintf(stderr, COLOR_RED "\nDEBUG: invalid opcode\n" COLOR_GREEN);
            return false;
        }
        else if (r == MCS6502ExecResultHalting)
        {
            // ...
        }
    }
    
    return true;
}