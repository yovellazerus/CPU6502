#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

// Reset
#define COLOR_RESET        "\x1b[0m"
// Regular text colors (foreground)
#define COLOR_BLACK        "\x1b[30m"
#define COLOR_RED          "\x1b[31m"
#define COLOR_GREEN        "\x1b[32m"
#define COLOR_YELLOW       "\x1b[33m"
#define COLOR_BLUE         "\x1b[34m"
#define COLOR_MAGENTA      "\x1b[35m"
#define COLOR_CYAN         "\x1b[36m"
#define COLOR_WHITE        "\x1b[37m"
// Bright text colors (foreground)
#define COLOR_BRIGHT_BLACK    "\x1b[90m"
#define COLOR_BRIGHT_RED      "\x1b[91m"
#define COLOR_BRIGHT_GREEN    "\x1b[92m"
#define COLOR_BRIGHT_YELLOW   "\x1b[93m"
#define COLOR_BRIGHT_BLUE     "\x1b[94m"
#define COLOR_BRIGHT_MAGENTA  "\x1b[95m"
#define COLOR_BRIGHT_CYAN     "\x1b[96m"
#define COLOR_BRIGHT_WHITE    "\x1b[97m"
// Background colors
#define COLOR_BG_BLACK      "\x1b[40m"
#define COLOR_BG_RED        "\x1b[41m"
#define COLOR_BG_GREEN      "\x1b[42m"
#define COLOR_BG_YELLOW     "\x1b[43m"
#define COLOR_BG_BLUE       "\x1b[44m"
#define COLOR_BG_MAGENTA    "\x1b[45m"
#define COLOR_BG_CYAN       "\x1b[46m"
#define COLOR_BG_WHITE      "\x1b[47m"
// Bright background colors
#define COLOR_BG_BRIGHT_BLACK   "\x1b[100m"
#define COLOR_BG_BRIGHT_RED     "\x1b[101m"
#define COLOR_BG_BRIGHT_GREEN   "\x1b[102m"
#define COLOR_BG_BRIGHT_YELLOW  "\x1b[103m"
#define COLOR_BG_BRIGHT_BLUE    "\x1b[104m"
#define COLOR_BG_BRIGHT_MAGENTA "\x1b[105m"
#define COLOR_BG_BRIGHT_CYAN    "\x1b[106m"
#define COLOR_BG_BRIGHT_WHITE   "\x1b[107m"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define UNUSED() do                                                          \
{                                                                            \
    fprintf(stderr,                                                          \
        COLOR_YELLOW                                                         \
        "UNUSED: function: \"%s\" in file: \"%s\" is not implemented.\n"     \
        COLOR_RESET,                                                         \
        __func__, __FILE__);                                                 \
    exit(1);                                                                 \
} while (0);

#define CPU_ERROR(cpu, msg, ...) CPU_error(cpu, msg, ##__VA_ARGS__);                       \
                            fprintf(stderr, COLOR_RED "in: (%s : %s : %d)\n" COLOR_RESET,  \
                                    __FILE__, __func__, __LINE__)

typedef uint8_t byte;
typedef uint16_t word;

typedef struct {
    void* ctx;
    byte (*read)(void*, word);
    void (*write)(void*, word, byte);
} Bus_t;

typedef struct CPU_t CPU_t;

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

} Opcode_t;

typedef enum {
    Addressing_mode_Brk = 0,
    Addressing_mode_Relative,
    Addressing_mode_Implied,
    Addressing_mode_Immediate,
    Addressing_mode_Accumulator,

    Addressing_mode_Absolute,
    Addressing_mode_AbsoluteX,
    Addressing_mode_AbsoluteY,

    Addressing_mode_ZeroPage,
    Addressing_mode_ZeroPageX,
    Addressing_mode_ZeroPageY,

    Addressing_mode_Indirect,
    Addressing_mode_IndirectX,
    Addressing_mode_IndirectY,

} Addressing_mode_t;

CPU_t* CPU_create(Bus_t bus);
void   CPU_destroy(CPU_t* cpu);
byte   CPU_read(CPU_t* cpu, word addr);
void   CPU_write(CPU_t* cpu, word addr, byte value);
bool   CPU_step(CPU_t* cpu);
void   CPU_error(CPU_t* cpu, const char* fmt, ...);
void   CPU_dump_registers(CPU_t* cpu, FILE* file);
void   CPU_reset(CPU_t* cpu);
void   CPU_nmi(CPU_t* cpu);
void   CPU_irq(CPU_t* cpu);

#endif // CPU_H_
