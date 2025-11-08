#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

#include "ansi_codes.h"

#ifdef _WIN64
#include <conio.h>
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define UNUSED (void)cpu; fprintf(stderr, COLOR_RED "ERROR: function: %s is not implemented." COLOR_RESET, __func__); exit(1);

#define HIGH_BYTE(WORD) ((byte)(WORD >> 8))
#define LOW_BYTE(WORD) ((byte)(WORD))
#define IS_CROSS_PAGES(old, _new) ((old & 0xFF00) != (_new & 0xFF00))

#define IS_ZERO(reg) (reg == 0)
#define IS_NEGATIVE(reg) ((reg & 0x80) != 0)
#define IS_OVERFLOW(reg, operand) (((reg ^ (reg + operand)) & ((operand ^ (reg + operand)) & 0x80)) != 0)
#define IS_CARRY(reg, operand) (HIGH_BYTE(reg + operand) > 0)

#define MEMORY_SIZE (256*256)
#define ZERO_PAGE_START 0x0000
#define ZERO_PAGE_END 0x00ff
#define STACK_START 0x0100
#define STACK_END 0x01ff
#define PAGE_SIZE 0x0100

#define BLOCK_SIZE 0x0100
#define DISK_BLOCK_COUNT (8*256)
#define DISK_TIME 100

#define RESET_SP_REGISTER 0xfd

/****     MMIO and flags for the CPU, with consisted with bios.asm  ****/

// flags:
#define POWER_OFF 0xFF

#define DISK_BAD 0
#define DISK_READY 1
#define DISK_ERR 2
#define DISK_PRESENT 0xFF

#define DISK_IDLE 0
#define DISK_READ 1
#define DISK_WRITE 2

// devices:

// wild-card devices
#define NULL_REG      0xC000
#define POWER         0xC001
#define RND           0xC002
// KERNEL status
#define BOOT_STATUS   0xC003
// dick ctrl
#define DISK_CMD      0xC004
#define DISK_ADDRL    0xC005
#define DISK_ADDRH    0xC006
#define DISK_STATUS   0xC007
// I/O:
#define KEB_DATA      0xC008
#define KEB_CTRL      0xC009
#define SCR_DATA      0xC00A
#define SCR_CTRL      0xC00B
#define SPK_DATA      0xC00C
#define SPK_CTRL      0xC00D
#define MIC_DATA      0xC00E
#define MIC_CTRL      0xC00F
// MMU
#define BSL           0xC010
#define BSH           0xC011
#define SSL           0xC012
#define SSH           0xC013
// dick data
#define DISK_DATA     0xC100 // 256 byte blocks

//****************************************************************************

// for WosMon:
#define APPLE1_KBD     0xD010   // PIA.A keyboard input
#define APPLE1_KBDCR   0xD011   // PIA.A keyboard control register
#define APPLE1_DSP     0xD012   // PIA.B display output register
#define APPLE1_DSPCR   0xD013   // PIA.B display control register

//****************************************************************************

typedef uint8_t byte;
typedef uint16_t word;

typedef enum {
    Add_non = 0,
    Add_Brk,
    Add_Relative,
    Add_Implied,
    Add_Immediate,
    Add_Accumulator,

    Add_Absolute,
    Add_AbsoluteX,
    Add_AbsoluteY,

    Add_ZeroPage,
    Add_ZeroPageX,
    Add_ZeroPageY,

    Add_Indirect,
    Add_IndirectX,
    Add_IndirectY,

    
    count_Add,
} Addressing_mode;

extern const char* Addressing_mode_to_cstr[count_Add + 1];

typedef enum {
    // Load/Store Operations
    Opcode_LDA_Immediate   = 0xA9,
    Opcode_LDA_ZeroPage    = 0xA5,
    Opcode_LDA_ZeroPageX   = 0xB5,
    Opcode_LDA_Absolute    = 0xAD,
    Opcode_LDA_AbsoluteX   = 0xBD,
    Opcode_LDA_AbsoluteY   = 0xB9,
    Opcode_LDA_IndirectX   = 0xA1,
    Opcode_LDA_IndirectY   = 0xB1,

    Opcode_LDX_Immediate   = 0xA2,
    Opcode_LDX_ZeroPage    = 0xA6,
    Opcode_LDX_ZeroPageY   = 0xB6,
    Opcode_LDX_Absolute    = 0xAE,
    Opcode_LDX_AbsoluteY   = 0xBE,

    Opcode_LDY_Immediate   = 0xA0,
    Opcode_LDY_ZeroPage    = 0xA4,
    Opcode_LDY_ZeroPageX   = 0xB4,
    Opcode_LDY_Absolute    = 0xAC,
    Opcode_LDY_AbsoluteX   = 0xBC,

    Opcode_STA_ZeroPage    = 0x85,
    Opcode_STA_ZeroPageX   = 0x95,
    Opcode_STA_Absolute    = 0x8D,
    Opcode_STA_AbsoluteX   = 0x9D,
    Opcode_STA_AbsoluteY   = 0x99,
    Opcode_STA_IndirectX   = 0x81,
    Opcode_STA_IndirectY   = 0x91,

    Opcode_STX_ZeroPage    = 0x86,
    Opcode_STX_ZeroPageY   = 0x96,
    Opcode_STX_Absolute    = 0x8E,

    Opcode_STY_ZeroPage    = 0x84,
    Opcode_STY_ZeroPageX   = 0x94,
    Opcode_STY_Absolute    = 0x8C,

    // Register Transfers
    Opcode_TAX             = 0xAA,
    Opcode_TAY             = 0xA8,
    Opcode_TSX             = 0xBA,
    Opcode_TXA             = 0x8A,
    Opcode_TXS             = 0x9A,
    Opcode_TYA             = 0x98,

    // Stack Operations
    Opcode_PHA             = 0x48,
    Opcode_PHP             = 0x08,
    Opcode_PLA             = 0x68,
    Opcode_PLP             = 0x28,

    // Logical
    Opcode_AND_Immediate   = 0x29,
    Opcode_AND_ZeroPage    = 0x25,
    Opcode_AND_ZeroPageX   = 0x35,
    Opcode_AND_Absolute    = 0x2D,
    Opcode_AND_AbsoluteX   = 0x3D,
    Opcode_AND_AbsoluteY   = 0x39,
    Opcode_AND_IndirectX   = 0x21,
    Opcode_AND_IndirectY   = 0x31,

    Opcode_EOR_Immediate   = 0x49,
    Opcode_EOR_ZeroPage    = 0x45,
    Opcode_EOR_ZeroPageX   = 0x55,
    Opcode_EOR_Absolute    = 0x4D,
    Opcode_EOR_AbsoluteX   = 0x5D,
    Opcode_EOR_AbsoluteY   = 0x59,
    Opcode_EOR_IndirectX   = 0x41,
    Opcode_EOR_IndirectY   = 0x51,

    Opcode_ORA_Immediate   = 0x09,
    Opcode_ORA_ZeroPage    = 0x05,
    Opcode_ORA_ZeroPageX   = 0x15,
    Opcode_ORA_Absolute    = 0x0D,
    Opcode_ORA_AbsoluteX   = 0x1D,
    Opcode_ORA_AbsoluteY   = 0x19,
    Opcode_ORA_IndirectX   = 0x01,
    Opcode_ORA_IndirectY   = 0x11,

    Opcode_BIT_ZeroPage    = 0x24,
    Opcode_BIT_Absolute    = 0x2C,

    // Arithmetic
    Opcode_ADC_Immediate   = 0x69,
    Opcode_ADC_ZeroPage    = 0x65,
    Opcode_ADC_ZeroPageX   = 0x75,
    Opcode_ADC_Absolute    = 0x6D,
    Opcode_ADC_AbsoluteX   = 0x7D,
    Opcode_ADC_AbsoluteY   = 0x79,
    Opcode_ADC_IndirectX   = 0x61,
    Opcode_ADC_IndirectY   = 0x71,

    Opcode_SBC_Immediate   = 0xE9,
    Opcode_SBC_ZeroPage    = 0xE5,
    Opcode_SBC_ZeroPageX   = 0xF5,
    Opcode_SBC_Absolute    = 0xED,
    Opcode_SBC_AbsoluteX   = 0xFD,
    Opcode_SBC_AbsoluteY   = 0xF9,
    Opcode_SBC_IndirectX   = 0xE1,
    Opcode_SBC_IndirectY   = 0xF1,

    Opcode_CMP_Immediate   = 0xC9,
    Opcode_CMP_ZeroPage    = 0xC5,
    Opcode_CMP_ZeroPageX   = 0xD5,
    Opcode_CMP_Absolute    = 0xCD,
    Opcode_CMP_AbsoluteX   = 0xDD,
    Opcode_CMP_AbsoluteY   = 0xD9,
    Opcode_CMP_IndirectX   = 0xC1,
    Opcode_CMP_IndirectY   = 0xD1,

    Opcode_CPX_Immediate   = 0xE0,
    Opcode_CPX_ZeroPage    = 0xE4,
    Opcode_CPX_Absolute    = 0xEC,

    Opcode_CPY_Immediate   = 0xC0,
    Opcode_CPY_ZeroPage    = 0xC4,
    Opcode_CPY_Absolute    = 0xCC,

    // Increments & Decrements
    Opcode_INC_ZeroPage    = 0xE6,
    Opcode_INC_ZeroPageX   = 0xF6,
    Opcode_INC_Absolute    = 0xEE,
    Opcode_INC_AbsoluteX   = 0xFE,

    Opcode_INX             = 0xE8,
    Opcode_INY             = 0xC8,

    Opcode_DEC_ZeroPage    = 0xC6,
    Opcode_DEC_ZeroPageX   = 0xD6,
    Opcode_DEC_Absolute    = 0xCE,
    Opcode_DEC_AbsoluteX   = 0xDE,

    Opcode_DEX             = 0xCA,
    Opcode_DEY             = 0x88,

    // Shifts
    Opcode_ASL_Accumulator = 0x0A,
    Opcode_ASL_ZeroPage    = 0x06,
    Opcode_ASL_ZeroPageX   = 0x16,
    Opcode_ASL_Absolute    = 0x0E,
    Opcode_ASL_AbsoluteX   = 0x1E,

    Opcode_LSR_Accumulator = 0x4A,
    Opcode_LSR_ZeroPage    = 0x46,
    Opcode_LSR_ZeroPageX   = 0x56,
    Opcode_LSR_Absolute    = 0x4E,
    Opcode_LSR_AbsoluteX   = 0x5E,

    Opcode_ROL_Accumulator = 0x2A,
    Opcode_ROL_ZeroPage    = 0x26,
    Opcode_ROL_ZeroPageX   = 0x36,
    Opcode_ROL_Absolute    = 0x2E,
    Opcode_ROL_AbsoluteX   = 0x3E,

    Opcode_ROR_Accumulator = 0x6A,
    Opcode_ROR_ZeroPage    = 0x66,
    Opcode_ROR_ZeroPageX   = 0x76,
    Opcode_ROR_Absolute    = 0x6E,
    Opcode_ROR_AbsoluteX   = 0x7E,

    // Jumps & Calls
    Opcode_JMP_Absolute    = 0x4C,
    Opcode_JMP_Indirect    = 0x6C,
    Opcode_JSR             = 0x20,
    Opcode_RTS             = 0x60,

    // Branches
    Opcode_BCC             = 0x90,
    Opcode_BCS             = 0xB0,
    Opcode_BEQ             = 0xF0,
    Opcode_BMI             = 0x30,
    Opcode_BNE             = 0xD0,
    Opcode_BPL             = 0x10,
    Opcode_BVC             = 0x50,
    Opcode_BVS             = 0x70,

    // Status Flag Changes
    Opcode_CLC             = 0x18,
    Opcode_CLD             = 0xD8,
    Opcode_CLI             = 0x58,
    Opcode_CLV             = 0xB8,
    Opcode_SEC             = 0x38,
    Opcode_SED             = 0xF8,
    Opcode_SEI             = 0x78,

    // System
    Opcode_BRK             = 0x00,
    Opcode_NOP             = 0xEA,
    Opcode_RTI             = 0x40,

} Opcode;

typedef struct CPU_t {

    byte memory[MEMORY_SIZE];

    word PC;
    byte SP;
    byte P;

    byte A;
    byte X;
    byte Y;

} CPU;

typedef void (*Instruction)(CPU*);

extern Instruction Opcode_to_Instruction_table[0xff + 1];

// dump's for debug
void CPU_dump_cpu(CPU* cpu, FILE* file);
void CPU_dump_memory(CPU* cpu, FILE* file);
void CPU_dump_stack(CPU* cpu, FILE* file);
void CPU_dumpProgram(CPU* cpu, word entry_point, size_t program_size, FILE* file);
bool CPU_debug(CPU* cpu);

// helpers functions for cpu 
bool CPU_offFlag(CPU* cpu, char flag);
bool CPU_onFlag(CPU* cpu, char flag);
bool CPU_getFlag(CPU* cpu, char flag);
void CPU_updateFlags(CPU* cpu, char reg, char flag, byte old_value, byte operand);
void CPU_push(CPU* cpu, char reg);
void CPU_pop(CPU* cpu, char reg);
void CPU_tick(CPU* cpu, size_t amount); 
void CPU_invalid_opcode(CPU* cpu, byte opcode);

// hardware input
void CPU_reset(CPU* cpu);
void CPU_nmi(CPU* cpu);
void CPU_irq(CPU* cpu);

// main emulator function
void CPU_run(CPU* cpu, byte disk[DISK_BLOCK_COUNT][BLOCK_SIZE]);

// I/O
bool CPU_power(CPU* cpu);
void CPU_keyboard(CPU* cpu);
void CPU_screen(CPU* cpu);
void CPU_disk(CPU* cpu, byte disk[DISK_BLOCK_COUNT][BLOCK_SIZE]);

// for WosMon:
bool CPU_apple_1_IO_sim(CPU* cpu);

#endif // CPU_H_
