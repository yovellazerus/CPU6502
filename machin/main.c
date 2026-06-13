
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <conio.h>

#include "MCS6502.h"
#include "ansi_codes.h"

/* ======== header ===========================================================*/

#define RAM_BASE     0x0000
#define RAM_SIZE     0xfc00

#define DISK_BUF     0xfc00 // 512 bytes
#define DISK_STAT    0xfe00
#define DISK_CMD     0xfe01
#define DISK_LBA     0xfe02

#define UART_TX      0xfe10
#define UART_RX      0xfe11

#define MMU_MAP      0xfe20 // 16 bytes

#define ROM_BASE     0xff00
#define ROM_SIZE     0x0100

typedef MCS6502ExecutionContext CPU;

typedef struct {
    uint8_t tx;
    uint8_t rx;
} Uart;

enum {
    NO_DISK   = 0,
    CMD_READ  = 1,
    STAT_READY = 2,
    BAD_LBA   = 3,
    MAX_LBA   = (64*1024 / 512),
};

typedef struct {
    FILE* file;
    uint8_t buffer[512];
    uint8_t cmd;
    uint8_t status;
    uint8_t lba_low;
    uint8_t lba_high;
} Disk;

typedef struct {
      
    CPU* cpu;
    uint8_t* ram;
    uint8_t* rom;
    Uart* uart;
    Disk* disk;
    // ...

} Machin;

uint8_t Machin_read(uint16_t addr, void* ctx);
void    Machin_write(uint16_t addr, uint8_t byte, void* ctx);

Machin* Machin_create(const char* rom_path, const char* disk_path);
void    Machin_destroy(Machin* m);
void    Machin_coredump(const Machin* m, const char* path);
bool    Machin_step(Machin* m);

/* ======== main =============================================================*/

int main(void)
{
    const char* disk_path = "disk.bin";
    const char* rom_path = "rom.bin";
    
    Machin* m = Machin_create(rom_path, disk_path);
    if(!m) return 1;
    while(Machin_step(m));
    Machin_coredump(m, "coredump.bin");
    Machin_destroy(m);

    return 0;
}

/* ======== read/write =======================================================*/

// TODO: MMU...
uint8_t Machin_read(uint16_t addr, void* ctx) {
    if(!ctx) return 0xFF;
    Machin* m = (Machin*)ctx;

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

    // ---------------- UART ----------------
    if (addr == UART_RX)
    {
        uint8_t c = m->uart->rx;
        m->uart->rx = 0;
        return c;
    }

    if (addr == UART_TX)
        return m->uart->tx;

    // ---------------- ROM ----------------
    if (addr >= ROM_BASE)
        return m->rom[addr - ROM_BASE];

    // ---------------- unmapped ----------------
    return 0xFF;
}

// TODO: MMU...
void Machin_write(uint16_t addr, uint8_t byte, void* ctx) {
    if(!ctx) return;
    Machin* m = (Machin*)ctx;

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
        return;
    }
    
    // ---------------- DISK LBA ----------------
    if (addr == DISK_LBA)
    {
        m->disk->lba_low = byte;
        return;
    }
    
    if (addr == DISK_LBA+1)
    {
        m->disk->lba_high = byte;
        return;
    }

    // ---------------- UART TX ----------------
    if (addr == UART_TX)
    {
        m->uart->tx = byte;
        return;
    }
}

/* ======== functions ========================================================*/

Machin* Machin_create(const char* rom_path, const char* disk_path) {

    Machin* m = (Machin*)calloc(1, sizeof(Machin));
    if (!m) return NULL;

    m->cpu = (CPU*)calloc(1, sizeof(CPU));
    m->ram = (uint8_t*)calloc(RAM_SIZE, sizeof(uint8_t));
    m->rom = (uint8_t*)calloc(ROM_SIZE, sizeof(uint8_t));
    m->uart = (Uart*)calloc(1, sizeof(Uart));
    m->disk = (Disk*)calloc(1, sizeof(Disk));

    if (!m->ram || !m->rom || !m->uart || !m->disk || !m->cpu){
        Machin_destroy(m);
        return NULL;
    }
        
    // ROM
    FILE* rom_img = fopen(rom_path, "rb");
    if(!rom_img){
        Machin_destroy(m);
        return NULL;
    }
    ssize_t rom_size = fread(m->rom, 1, ROM_SIZE, rom_img);
    if(rom_size > ROM_SIZE){
        fclose(rom_img);
        Machin_destroy(m);
        return NULL; 
    }
    fclose(rom_img);
    
    // disk
    if(disk_path){
       m->disk->file = fopen(disk_path, "rb+");
       if(!m->disk->file){
           Machin_destroy(m);
           return NULL;
       }
       m->disk->status = ~NO_DISK; 
    }
    
    // CPU
    MCS6502Init(
        m->cpu,
        Machin_read,
        Machin_write,
        m
    );
    MCS6502Reset(m->cpu);
    
    // uart
    printf(COLOR_GREEN);

    return m;
}

void Machin_destroy(Machin* m) {
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

void Machin_coredump(const Machin* m, const char* path) {
    if(!m || !path) return;
    
    FILE* f = fopen(path, "wb");
    if (!f) return;

    uint8_t mmio[256];
    memset(mmio, 'd', sizeof(mmio));
    
    fwrite(m->ram, 1, RAM_SIZE, f);
    fwrite(m->disk->buffer, 1, 512, f);
    fwrite(mmio, 1, sizeof(mmio), f);
    fwrite(m->rom, 1, ROM_SIZE, f);
    
    fclose(f);
}

bool Machin_step(Machin* m){
    if(!m) return false;
    
    // keyboard
    if (_kbhit())
    {
        int c = _getch();
        
        // ESC 
        if(c == 0x1B){
            return false; // power-off
        }
        
        if (c != -1)
            m->uart->rx = (uint8_t)c;
    }
    
    // display
    if(m->uart->tx != 0){
        putchar(m->uart->tx);
        fflush(stdout);
        m->uart->tx = 0;
    }
    
    // disk
    // TODO: time delay
    if(m->disk->cmd == CMD_READ){
        m->disk->cmd = 0;
        uint16_t lba = (m->disk->lba_high << 8) | m->disk->lba_low;
        if(lba > MAX_LBA){
            m->disk->status = BAD_LBA;
        }
        else{
            fseek(m->disk->file, lba*512, SEEK_SET);
            fread(m->disk->buffer, 1, 512, m->disk->file);
            m->disk->status = STAT_READY;
        }
    }
    
    // CPU
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
    
    return true;
}