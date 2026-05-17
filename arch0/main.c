
// =============================================================================
// my 6502 computer!
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <conio.h>

#include "MCS6502.h"

#define RAM_BASE  0x0000
#define RAM_SIZE  0xf800

#define DISK_BUF   0xfc00
#define DISK_CMD   0xfe00
#define DISK_STAT  0xfe01

#define UART_RX 0xfe02
#define UART_TX 0xfe03

#define ROM_BASE  0xff00
#define ROM_SIZE  0x0100

typedef MCS6502ExecutionContext CPU;

typedef struct {
    uint8 tx;
    uint8 rx;
} Uart;

typedef struct {
    uint8 buffer[512];
    uint8 cmd;
    uint8 status;
} Disk;

typedef struct {
    CPU* cpu;
    uint8* rom;
    uint8* ram;
    Uart* uart;
    Disk* disk;
} Machin;

uint8 Machin_read(uint16 addr, void * ctx);
void  Machin_write(uint16 addr, uint8 byte, void * ctx);
Machin* Machin_create(void);
void Machin_destroy(Machin* self);
void Machin_coredump(const Machin* self, const char* path);
void Machin_run(Machin* self);

int main(void)
{
    Machin* m = Machin_create();
    if (!m)
    {
        fprintf(stderr, "Failed to create machine\n");
        return 1;
    }

    printf("Starting 6502 machine...\n");
    Machin_run(m);
    Machin_coredump(m, "ram.bin");
    Machin_destroy(m);

    return 0;
}

uint8 Machin_read(uint16 addr, void * ctx)
{
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
        uint8 c = m->uart->rx;
        m->uart->rx = 0; // simple "consume"
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

void Machin_write(uint16 addr, uint8 byte, void * ctx)
{
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

    // ---------------- UART TX ----------------
    if (addr == UART_TX)
    {
        if (byte == '\r') byte = '\n';
        putchar(byte);   // output to terminal
        fflush(stdout);
        return;
    }

    // ---------------- ROM (ignore writes) ----------------
}

Machin* Machin_create(void)
{
    Machin* m = (Machin*)calloc(1, sizeof(Machin));
    if (!m) return NULL;

    m->cpu = (CPU*)calloc(1, sizeof(CPU));
    m->ram = (uint8*)calloc(RAM_SIZE, sizeof(uint8));
    m->rom = (uint8*)calloc(ROM_SIZE, sizeof(uint8));
    m->uart = (Uart*)calloc(1, sizeof(Uart));
    m->disk = (Disk*)calloc(1, sizeof(Disk));

    if (!m->ram || !m->rom || !m->uart || !m->disk || !m->cpu)
        return NULL;

    FILE* romf = fopen("rom.bin", "rb");
    if (romf)
    {
        fread(m->rom, 1, ROM_SIZE, romf);
        fclose(romf);
        printf("ROM loaded\n");

    }
    else
    {
        printf("No ROM found (running empty memory)\n");
    }
 
    MCS6502Init(
        m->cpu,
        Machin_read,
        Machin_write,
        m
    );

    MCS6502Reset(m->cpu);
    
    return m;
}

void Machin_destroy(Machin* self)
{
    if (!self) return;

    free(self->ram);
    free(self->rom);
    free(self->uart);
    free(self->disk);
    free(self->cpu);
    free(self);
}

void Machin_run(Machin* self)
{
    while (1)
    {
        // inject keyboard into UART RX
        if (_kbhit())
        {
            int c = _getch();
            if (c != -1)
                self->uart->rx = (uint8)c;
            if(c == 'q') Machin_coredump(self, "ram.bin");
        }

        // run CPU
        MCS6502ExecResult r = MCS6502ExecNext(self->cpu);

        if (r == MCS6502ExecResultInvalidOperation)
        {
            printf("\n[CPU HALT: invalid opcode]\n");
            break;
        }

        if (r == MCS6502ExecResultHalting)
        {
            // optional idle behavior
        }
    }
}

void Machin_coredump(const Machin* self, const char* path)
{
    FILE* f = fopen(path, "wb");
    if (!f) return;

    fwrite(self->ram, 1, RAM_SIZE, f);
    
    fclose(f);
}