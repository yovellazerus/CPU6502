
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// windows only
#include <conio.h>

#include "MCS6502.h"
#include "ansi_codes.h"
#include "machine.h"

/* ========================================================== MMIO devices =========================================*/

typedef struct Machine Machine;
typedef MCS6502ExecutionContext CPU;

typedef struct ROM
{
    uint8_t  rom_enable;
    uint8_t* data;

} ROM;

typedef struct RAM
{
    uint8_t* data;

} RAM;


typedef struct Uart 
{
    Machine* m;
    uint8_t tx;
    uint8_t rx;
    uint8_t status;

} Uart;

typedef struct Disk 
{
    Machine* m;
    FILE* file;
    uint8_t buffer[DISK_SECTOR_SIZE];
    uint8_t cmd;
    uint8_t status;
    uint8_t lba_low;
    uint8_t lba_high;
    int delay;   // cycles remaining for operation

} Disk;

typedef struct MMU 
{
    Machine* m;
    uint8_t page_table[MMU_PAGE_TABLE_SIZE];

} MMU;

typedef struct Timer 
{
    Machine* m;
    uint8_t ctrl;
    uint8_t latch_low;
    uint8_t latch_high;
    uint8_t counter_low;
    uint8_t counter_high;  

} Timer;

// the Machine itself
struct Machine 
{

    bool pending_irq;

    // bus
    MCS6502DataReadByteFunction  read;
    MCS6502DataWriteByteFunction write;
      
    CPU* cpu;
    RAM* ram;
    ROM*  rom;
    Uart* uart;
    Disk* disk;
    MMU*  mmu;
    Timer* timer;

    // ...

};

/* ================================================= helpers functions ======================================================*/

static uint32_t mmu_translate(MMU* mmu, uint16_t va) {
    uint8_t segment = va >> 12;          
    uint16_t offset = va & 0x0FFF;       
    uint8_t frame = mmu->page_table[segment];

    if(frame == MMU_FRAME_INVALID){
        
        // debug
        fprintf(stderr, COLOR_RED "\nMMU:\n");
        fprintf(stderr, "PC = 0x%.4x\n", mmu->m->cpu->pc);
        fprintf(stderr, "unmap virtual address: 0x%.4x\n", va);
        for(int i = 0; i < 16; i++) printf("0x%.2x  ", mmu->page_table[i]);
        fprintf(stderr, "\n\n" COLOR_GREEN);

        // TODO: trigger an IRQ on memory access violation
        // m->pending_irq = true
        
        return -1;  // max uint32_t to be unmapped and so retrun 0xff for reading and ignored for writing

    }

    return (frame << 12) | offset;
}

/* ============================================== read/write functions ======================================================*/

uint8_t Machine_read(uint16_t addr, void* ctx) {
    if(!ctx) return 0xFF;
    Machine* m = (Machine*)ctx;

    // It is essential for interrupts to feach the vector from the trap frame
    if (addr >= 0xfffa && m->pending_irq) {
        m->mmu->page_table[MMU_LAST_SEGMENT] = MMU_LAST_SEGMENT;
        m->pending_irq = false;
    }

    uint32_t physical_addr = mmu_translate(m->mmu, addr);

    // ---------------- TIMER ----------------
    if(physical_addr == TIMER_CTRL) return m->timer->ctrl;
    if(physical_addr == TIMER_LATCH_HIGH) return m->timer->latch_high;
    if(physical_addr == TIMER_LATCH_LOW) return m->timer->latch_low;
    if(physical_addr == TIMER_COUNTER_HIGH) return m->timer->counter_high;
    if(physical_addr == TIMER_COUNTER_LOW) return m->timer->counter_low;


    // ---------------- MMU ----------------
    if (physical_addr >= MMU_PAGE_TABLE && physical_addr < MMU_PAGE_TABLE + sizeof(m->mmu->page_table))
        return m->mmu->page_table[physical_addr - MMU_PAGE_TABLE];

    // ---------------- ROM ----------------
    if (physical_addr == ROM_ENABLE) return m->rom->rom_enable;
    if (physical_addr >= ROM_BASE && physical_addr < ROM_BASE + ROM_SIZE) {
        if (m->rom->rom_enable) {
            return m->rom->data[physical_addr - ROM_BASE];
        }
        // If rom_enable is false, fall through to read the RAM underneath it
    }

    // ---------------- DISK buffer ----------------
    if (physical_addr >= DISK_BUF && physical_addr < DISK_BUF + sizeof(m->disk->buffer))
        return m->disk->buffer[physical_addr - DISK_BUF];

    // ---------------- DISK registers ----------------
    if (physical_addr == DISK_CMD)
        return m->disk->cmd;

    if (physical_addr == DISK_STAT)
        return m->disk->status;

    if (physical_addr == DISK_LBA + 0)
        return m->disk->lba_low;

    if (physical_addr == DISK_LBA + 1)
        return m->disk->lba_high;

    // ---------------- UART ----------------
    if (physical_addr == UART_RX)
    {
        uint8_t c = m->uart->rx;
        m->uart->status &= ~UART_STATUS_RX_READY;
        return c;
    }

    if (physical_addr == UART_TX)
        return m->uart->tx;

    if (physical_addr == UART_STAT)
        return m->uart->status;

    // MUST BE LAST!
    // ---------------- RAM ----------------
    if (physical_addr < RAM_SIZE)
        return m->ram->data[physical_addr];

    // ---------------- unmapped ----------------
    return 0xFF;
}

void Machine_write(uint16_t addr, uint8_t byte, void* ctx) {
    if(!ctx) return;
    Machine* m = (Machine*)ctx;

    uint32_t physical_addr = mmu_translate(m->mmu, addr);

    // ---------------- TIMER ----------------
    if(physical_addr == TIMER_CTRL){
        m->timer->ctrl = byte;
        
        if (byte == TIMER_ENABLE_TRUE) {
            m->timer->counter_high = m->timer->latch_high;
            m->timer->counter_low = m->timer->latch_low;
        }
        return;
    }
    if(physical_addr == TIMER_LATCH_HIGH){
        m->timer->latch_high = byte;
        return;
    }
    if(physical_addr == TIMER_LATCH_LOW){
        m->timer->latch_low = byte;
        return;
    }
    if(physical_addr == TIMER_COUNTER_HIGH){
        m->timer->counter_high = byte;
        return;
    }
    if(physical_addr == TIMER_COUNTER_LOW){
        m->timer->counter_low = byte;
        return;
    }

    // ---------------- MMU ----------------
    if (physical_addr >= MMU_PAGE_TABLE && physical_addr < MMU_PAGE_TABLE + sizeof(m->mmu->page_table)){
        m->mmu->page_table[physical_addr - MMU_PAGE_TABLE] = byte;
        return;
    }

    // ---------------- ROM ENABLE ----------------
    if (physical_addr == ROM_ENABLE){
        // TODO: for debug, cant reenable the ROM
        if(m->rom->rom_enable == ROM_ENABLE_TRUE) m->rom->rom_enable = byte;
        return;
    } 

    // ---------------- DISK buffer ----------------
    if (physical_addr >= DISK_BUF && physical_addr < DISK_BUF + sizeof(m->disk->buffer))
    {
        m->disk->buffer[physical_addr - DISK_BUF] = byte;
        return;
    }

    // ---------------- DISK CMD ----------------
    if (physical_addr == DISK_CMD)
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
    if (physical_addr == DISK_LBA + 0)
    {
        m->disk->lba_low = byte;
        return;
    }
    
    if (physical_addr == DISK_LBA + 1)
    {
        m->disk->lba_high = byte;
        return;
    }

    // ---------------- UART TX ----------------
    if (physical_addr == UART_TX)
    {
        m->uart->tx = byte;
        m->uart->status &= ~UART_STATUS_TX_READY;
        return;
    }

    // MUST BE LAST
    // ---------------- RAM ----------------
    if (physical_addr < RAM_SIZE)
    {
        m->ram->data[physical_addr] = byte;
        return;
    }
}

/* =========================================== destructors ==================================================================*/

void CPU_destroy(CPU* cpu){
    free(cpu);
}

void ROM_destroy(ROM* rom){
    free(rom->data);
    free(rom);
}

void RAM_destroy(RAM* ram){
    free(ram->data);
    free(ram);
}

void Uart_destroy(Uart* uart){
    free(uart);
    printf(COLOR_RESET);
}

void Disk_destroy(Disk* disk){
    if(disk->file) fclose(disk->file);
    free(disk);
}

void MMU_destroy(MMU* mmu){
    free(mmu);
}

void Timer_destroy(Timer* timer){
    free(timer);
}

void Machine_destroy(Machine* m) {
    if (!m) return;
    CPU_destroy(m->cpu);
    ROM_destroy(m->rom);
    RAM_destroy(m->ram);
    Uart_destroy(m->uart);
    Disk_destroy(m->disk);
    MMU_destroy(m->mmu);
    Timer_destroy(m->timer);
    free(m);
}

/* ========================================= constructors ==================================================================*/

CPU* CPU_create(Machine* m){
    if(!m) return NULL;
    CPU* cpu = (CPU*)calloc(1, sizeof(CPU));
    if(!cpu) return NULL;
    MCS6502Init(
        cpu,
        m->read,
        m->write,
        m
    );
    MCS6502Reset(cpu);
    return cpu;
}

RAM* RAM_create(void){
    RAM* ram = (RAM*)calloc(1, sizeof(RAM));
    if(!ram){
        return NULL;
    }
    uint8_t* data = (uint8_t*)calloc(RAM_SIZE, sizeof(uint8_t));
    if(!data){
        free(ram);
        return NULL;
    }
    ram->data = data;
    return ram;
}

ROM* ROM_create(const char* path){
    ROM* rom = (ROM*)calloc(1, sizeof(ROM));
    if(!rom) return NULL;

    uint8_t* data = (uint8_t*)calloc(ROM_SIZE, sizeof(uint8_t));
    if(!data){
        free(rom);
        return NULL;
    } 

    FILE* rom_img = fopen(path, "rb");
    if(!rom_img){
        free(rom);
        free(data);
        return NULL;
    }
    ssize_t rom_size = fread(data, 1, ROM_SIZE, rom_img);
    if(rom_size > ROM_SIZE){
        fclose(rom_img);
        free(rom);
        free(data);
        return NULL;
    }
    fclose(rom_img);
    rom->data = data;
    rom->rom_enable = ROM_ENABLE_TRUE;
    return rom;
}

Uart* Uart_create(void){
    Uart* uart = (Uart*)calloc(1, sizeof(Uart));
    if(!uart) return NULL;
    uart->m = NULL;
    printf(COLOR_GREEN);
    return uart;
}

Disk* Disk_create(const char* path){
    Disk* disk = (Disk*)calloc(1, sizeof(Disk));
    if(!disk) return NULL;

    disk->m = NULL;
    if(path){
       disk->file = fopen(path, "rb+");
       if(!disk->file){
           free(disk);
           return NULL;
       }
       disk->status = ~DISK_STATUS_NONE; 
    }
    return disk;
}

MMU* MMU_create(void){
    MMU* mmu = (MMU*)calloc(1, sizeof(MMU));
    if(!mmu) return NULL;
    mmu->m = NULL;
    for (uint8_t i = 0; i < MMU_PAGE_TABLE_SIZE; i++) {
        mmu->page_table[i] = i; 
    }
    return mmu;
}

Timer* Timer_create(void){
    Timer* timer = (Timer*)calloc(1, sizeof(Timer));
    if(!timer) return NULL;
    timer->m = NULL;
    timer->ctrl = TIMER_ENABLE_FALSE;
    return timer;
}

Machine* Machine_create(const char* rom_path, const char* disk_path) {

    Machine* m = (Machine*)calloc(1, sizeof(Machine));
    if (!m) return NULL;

    m->pending_irq = false;

    // connecting the bus
    m->read = Machine_read;
    m->write = Machine_write;

    m->ram = RAM_create();
    m->rom = ROM_create(rom_path);
    m->uart = Uart_create();
    m->disk = Disk_create(disk_path);
    m->timer = Timer_create();
    m->mmu = MMU_create();
    m->cpu = CPU_create(m);
    
    if (!m->ram || !m->rom || !m->uart || !m->disk || !m->timer || !m->mmu || !m->cpu){
        Machine_destroy(m);
        return NULL;
    }

    // connecting the devices that can generate interrupts
    m->uart->m = m;
    m->disk->m = m;
    m->timer->m = m;
    m->mmu->m = m;

    return m;
}

/* =========================================== step functions ===============================================================*/

bool Disk_step(Disk* disk){
    if(!disk) return false;
    Machine* m = disk->m;
    if (m->disk->status == DISK_STATUS_BUSY)
    {
        if (--m->disk->delay == 0)
        {
            uint16_t lba = (m->disk->lba_high << 8) | m->disk->lba_low;

            if (m->disk->cmd == DISK_CMD_READ)
            {
                fseek(m->disk->file, lba * DISK_SECTOR_SIZE, SEEK_SET);
                fread(m->disk->buffer, 1, DISK_SECTOR_SIZE, m->disk->file);
            
                m->disk->status = DISK_STATUS_READY;
            }
            else if (m->disk->cmd == DISK_CMD_WRITE)
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
    return true;
}

bool Uart_step(Uart* uart){
    if(!uart) return false;

    // keyboard
    if (_kbhit())
    {
        int c = _getch();
        
        // ESC 
        if(c == 0x1B){
            return false; // power-off
        }

        if (c != -1){
            uart->m->uart->rx = (uint8_t)c;
            uart->m->uart->status |= UART_STATUS_RX_READY;
        }
    }

    // display
    if (!(uart->m->uart->status & UART_STATUS_TX_READY))
    {
        uint8_t byte = uart->m->uart->tx;
        if(byte == '\r') byte = '\n';
        _putch(byte);
        uart->m->uart->status |= UART_STATUS_TX_READY;
    }

    return true;
}

bool Timer_step(Timer* timer){
    if(!timer) return false; 
    
    if(timer->ctrl == TIMER_ENABLE_FALSE) return true;
    
    uint16_t counter = (uint16_t)timer->counter_high << 8;
    counter |= (uint16_t)timer->counter_low;
    
    if(--counter == 0){
    
        timer->counter_high = timer->latch_high;
        timer->counter_low = timer->latch_low;
        
        // Raise the hardware IRQ line
        // the IRQ itself is triggered in the CPU_step() function 
        timer->m->pending_irq = true;

        return true;
    }
    
    // commit the decremented value back to the counter
    timer->counter_high = (uint8_t)(counter >> 8);
    timer->counter_low = (uint8_t)(counter >> 0);
    
    return true;
}

// runing multiple cpu feach, decode, execute steps
bool CPU_step(CPU* cpu){
    if(!cpu) return false;
    Machine* m = (Machine*)cpu->readWriteContext;

    for(int i = 0; i < CPU_PER_STEP; i++){
        
        // The hardware IRQ trigger line
        if (m->pending_irq && !(m->cpu->p & MCS6502_STATUS_I)) {
            MCS6502IRQ(m->cpu); 
        }

        // hardware BRK Check simulating VPB pin
        if (m->read(cpu->pc, m) == 0x00) {
            m->mmu->page_table[MMU_LAST_SEGMENT] = MMU_LAST_SEGMENT; 
        }

        // an hardware "watchdog" for detecting user "SEI"/"PLP"/"RTI" feach cycle using SYNC pin
        // TODO: "RTI" is not implemented do to it been needed for closing the trap frame...
        if( (m->read(cpu->pc, m) == 0x78 || m->read(cpu->pc, m) == 0x28) && m->mmu->page_table[MMU_LAST_SEGMENT] != MMU_LAST_SEGMENT){
            m->mmu->page_table[MMU_LAST_SEGMENT] = MMU_LAST_SEGMENT; 
            MCS6502NMI(m->cpu);
        }
        
        // execute the instruction (or the interrupt sequence)
        MCS6502ExecResult result = MCS6502ExecNext(m->cpu);
    
        // debug
        if (result == MCS6502ExecResultInvalidOperation)
        {
            fprintf(stderr, COLOR_RED "\nCPU: \ninvalid opcode: 0x%.2x\nPC = 0x%.4x\n" COLOR_GREEN, m->read(cpu->pc, m), cpu->pc);
            return false;
        }
        else if (result == MCS6502ExecResultHalting)
        {
            // ...
        }
    }
    return true;
}

bool Machine_step(Machine* m){
    if(!m) return false;

    if(!Uart_step(m->uart)) return false; // power off
    (void)Disk_step(m->disk);
    (void)Timer_step(m->timer);
    if(!CPU_step(m->cpu)) return false; // invalid opcode
    
    return true;
}

/* ============================================== debug =====================================================================*/

void Machine_coredump(const Machine* m, const char* path) {
    if(!m || !path) return;
    FILE* f = fopen(path, "wb");
    if (!f) return;
    fwrite(m->ram->data, 1, RAM_SIZE, f);
    fclose(f);
}

/* =============================================== main =====================================================================*/

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
    Machine_coredump(m, ".\\machine\\coredump.bin");
    Machine_destroy(m);

    return 0;
}

