#ifndef LEXER_H_
#define LEXER_H_

#include <ctype.h>
#include <string.h>

#include "list.h"
#include "..\include\utilities.h"
#include "..\include\cpu.h"

const char* opcode_to_cstr[0xff + 1] = {

    [Opcode_LDA_Immediate]   = "LDA_I",
    [Opcode_LDA_ZeroPage ]   = "LDA_Z",
    [Opcode_LDA_ZeroPageX]   = "LDA_ZX",
    [Opcode_LDA_Absolute ]   = "LDA_A",
    [Opcode_LDA_AbsoluteX]   = "LDA_AX",
    [Opcode_LDA_AbsoluteY]   = "LDA_AY",
    [Opcode_LDA_IndirectX]   = "LDA_IX",
    [Opcode_LDA_IndirectY]   = "LDA_IY",
    [Opcode_LDX_Immediate]   = "LDX_I",
    [Opcode_LDX_ZeroPage ]   = "LDX_Z",
    [Opcode_LDX_ZeroPageY]   = "LDX_ZY",
    [Opcode_LDX_Absolute ]   = "LDX_A",
    [Opcode_LDX_AbsoluteY]   = "LDX_AY",
    [Opcode_LDY_Immediate]   = "LDY_I",
    [Opcode_LDY_ZeroPage ]   = "LDY_Z",
    [Opcode_LDY_ZeroPageX]   = "LDY_ZX",
    [Opcode_LDY_Absolute ]   = "LDY_A",
    [Opcode_LDY_AbsoluteX]   = "LDY_AX",
    [Opcode_STA_ZeroPage ]   = "STA_Z",
    [Opcode_STA_ZeroPageX]   = "STA_ZX",
    [Opcode_STA_Absolute ]   = "STA_A",
    [Opcode_STA_AbsoluteX]   = "STA_AX",
    [Opcode_STA_AbsoluteY]   = "STA_AY",
    [Opcode_STA_IndirectX]   = "STA_IX",
    [Opcode_STA_IndirectY]   = "STA_IY",
    [Opcode_STX_ZeroPage ]   = "STX_Z",
    [Opcode_STX_ZeroPageY]   = "STX_ZY",
    [Opcode_STX_Absolute ]   = "STX_A",
    [Opcode_STY_ZeroPage ]   = "STY_Z",
    [Opcode_STY_ZeroPageX]   = "STY_ZX",
    [Opcode_STY_Absolute ]   = "STY_A",
    [Opcode_TAX]             = "TAX",
    [Opcode_TAY]             = "TAY",
    [Opcode_TSX]             = "TSX",
    [Opcode_TXA]             = "TXA",
    [Opcode_TXS]             = "TXS",
    [Opcode_TYA]             = "TYA",
    [Opcode_PHA]             = "PHA",
    [Opcode_PHP]             = "PHP",
    [Opcode_PLA]             = "PLA",
    [Opcode_PLP]             = "PLP",
    [Opcode_AND_Immediate]   = "AND_I",
    [Opcode_AND_ZeroPage ]   = "AND_Z",
    [Opcode_AND_ZeroPageX]   = "AND_ZX",
    [Opcode_AND_Absolute ]   = "AND_A",
    [Opcode_AND_AbsoluteX]   = "AND_AX",
    [Opcode_AND_AbsoluteY]   = "AND_AY",
    [Opcode_AND_IndirectX]   = "AND_IX",
    [Opcode_AND_IndirectY]   = "AND_IY",
    [Opcode_EOR_Immediate]   = "EOR_I",
    [Opcode_EOR_ZeroPage ]   = "EOR_Z",
    [Opcode_EOR_ZeroPageX]   = "EOR_ZX",
    [Opcode_EOR_Absolute ]   = "EOR_A",
    [Opcode_EOR_AbsoluteX]   = "EOR_AX",
    [Opcode_EOR_AbsoluteY]   = "EOR_AY",
    [Opcode_EOR_IndirectX]   = "EOR_IX",
    [Opcode_EOR_IndirectY]   = "EOR_IY",
    [Opcode_ORA_Immediate]   = "ORA_I",
    [Opcode_ORA_ZeroPage ]   = "ORA_Z",
    [Opcode_ORA_ZeroPageX]   = "ORA_ZX",
    [Opcode_ORA_Absolute ]   = "ORA_A",
    [Opcode_ORA_AbsoluteX]   = "ORA_AX",
    [Opcode_ORA_AbsoluteY]   = "ORA_AY",
    [Opcode_ORA_IndirectX]   = "ORA_IX",
    [Opcode_ORA_IndirectY]   = "ORA_IY",
    [Opcode_BIT_ZeroPage ]   = "BIT_Z",
    [Opcode_BIT_Absolute ]   = "BIT_A",
    [Opcode_ADC_Immediate]   = "ADC_I",
    [Opcode_ADC_ZeroPage ]   = "ADC_Z",
    [Opcode_ADC_ZeroPageX]   = "ADC_ZX",
    [Opcode_ADC_Absolute ]   = "ADC_A",
    [Opcode_ADC_AbsoluteX]   = "ADC_AX",
    [Opcode_ADC_AbsoluteY]   = "ADC_AY",
    [Opcode_ADC_IndirectX]   = "ADC_IX",
    [Opcode_ADC_IndirectY]   = "ADC_IY",
    [Opcode_SBC_Immediate]   = "SBC_I",
    [Opcode_SBC_ZeroPage ]   = "SBC_Z",
    [Opcode_SBC_ZeroPageX]   = "SBC_ZX",
    [Opcode_SBC_Absolute ]   = "SBC_A",
    [Opcode_SBC_AbsoluteX]   = "SBC_AX",
    [Opcode_SBC_AbsoluteY]   = "SBC_AY",
    [Opcode_SBC_IndirectX]   = "SBC_IX",
    [Opcode_SBC_IndirectY]   = "SBC_IY",
    [Opcode_CMP_Immediate]   = "CMP_I",
    [Opcode_CMP_ZeroPage ]   = "CMP_Z",
    [Opcode_CMP_ZeroPageX]   = "CMP_ZX",
    [Opcode_CMP_Absolute ]   = "CMP_A",
    [Opcode_CMP_AbsoluteX]   = "CMP_AX",
    [Opcode_CMP_AbsoluteY]   = "CMP_AY",
    [Opcode_CMP_IndirectX]   = "CMP_IX",
    [Opcode_CMP_IndirectY]   = "CMP_I",
    [Opcode_CPX_Immediate]   = "CPX_I",
    [Opcode_CPX_ZeroPage ]   = "CPX_Z",
    [Opcode_CPX_Absolute ]   = "CPX_A",
    [Opcode_CPY_Immediate]   = "CPY_I",
    [Opcode_CPY_ZeroPage ]   = "CPY_Z",
    [Opcode_CPY_Absolute ]   = "CPY_A",
    [Opcode_INC_ZeroPage ]   = "INC_Z",
    [Opcode_INC_ZeroPageX]   = "INC_ZX",
    [Opcode_INC_Absolute ]   = "INC_A",
    [Opcode_INC_AbsoluteX]   = "INC_A",
    [Opcode_INX            ] = "INX",
    [Opcode_INY            ] = "INY",
    [Opcode_DEC_ZeroPage   ] = "DEC_Z",
    [Opcode_DEC_ZeroPageX  ] = "DEC_ZX",
    [Opcode_DEC_Absolute   ] = "DEC_A",
    [Opcode_DEC_AbsoluteX  ] = "DEC_AX",
    [Opcode_DEX            ] = "DEX",
    [Opcode_DEY            ] = "DEY",
    [Opcode_ASL_Accumulator] = "ASL_A",
    [Opcode_ASL_ZeroPage   ] = "ASL_Z",
    [Opcode_ASL_ZeroPageX  ] = "ASL_ZX",
    [Opcode_ASL_Absolute   ] = "ASL_A",
    [Opcode_ASL_AbsoluteX  ] = "ASL_A",
    [Opcode_LSR_Accumulator] = "LSR_A",
    [Opcode_LSR_ZeroPage   ] = "LSR_Z",
    [Opcode_LSR_ZeroPageX  ] = "LSR_ZX",
    [Opcode_LSR_Absolute   ] = "LSR_A",
    [Opcode_LSR_AbsoluteX  ] = "LSR_AX",
    [Opcode_ROL_Accumulator] = "ROL_A",
    [Opcode_ROL_ZeroPage   ] = "ROL_Z",
    [Opcode_ROL_ZeroPageX  ] = "ROL_ZX",
    [Opcode_ROL_Absolute   ] = "ROL_A",
    [Opcode_ROL_AbsoluteX  ] = "ROL_AX",
    [Opcode_ROR_Accumulator] = "ROR_A",
    [Opcode_ROR_ZeroPage   ] = "ROR_Z",
    [Opcode_ROR_ZeroPageX  ] = "ROR_ZX",
    [Opcode_ROR_Absolute   ] = "ROR_A",
    [Opcode_ROR_AbsoluteX  ] = "ROR_AX",
    [Opcode_JMP_Absolute   ] = "JMP_A",
    [Opcode_JMP_Indirect   ] = "JMP_I",
    [Opcode_JSR            ] = "JSR",
    [Opcode_RTS            ] = "RTS",
    [Opcode_BCC            ] = "BCC",
    [Opcode_BCS            ] = "BCS",
    [Opcode_BEQ            ] = "BEQ",
    [Opcode_BMI            ] = "BMI",
    [Opcode_BNE            ] = "BNE",
    [Opcode_BPL            ] = "BPL",
    [Opcode_BVC            ] = "BVC",
    [Opcode_BVS            ] = "BVS",
    [Opcode_CLC            ] = "CLC",
    [Opcode_CLD            ] = "CLD",
    [Opcode_CLI            ] = "CLI",
    [Opcode_CLV            ] = "CLV",
    [Opcode_SEC            ] = "SEC",
    [Opcode_SED            ] = "SED",
    [Opcode_SEI            ] = "SEI",
    [Opcode_BRK            ] = "BRK",
    [Opcode_NOP            ] = "NOP",
    [Opcode_RTI            ] = "RTI",
};

#define MAX_TOKENS 256

typedef enum {
    Token_eof,
    Token_err,

    Token_comment,

    Token_number,
    Token_char,
    Token_str,

    Token_alias,
    Token_label,
    Token_directive,
    Token_mnemonic,

    Token_hash,
    Token_eq,
    Token_open,
    Token_close,

} TokenType;

static const char* type_to_str_table[] = {
    [Token_eof] = "Token_eof",
    [Token_err] = "Token_err",
    [Token_comment] = "Token_comment",
    [Token_number] = "Token_number",
    [Token_char] = "Token_char",
    [Token_str] = "Token_str",
    [Token_alias] = "Token_alias",
    [Token_label] = "Token_label",
    [Token_directive] = "Token_directive",
    [Token_mnemonic] = "Token_mnemonic",
    [Token_hash] = "Token_hash",
    [Token_eq] = "Token_eq",
    [Token_open] = "Token_open",
    [Token_close] = "Token_close",
};

typedef struct Postion_t {
    size_t row;
    size_t coll;
    const char* file_name;
} Position;

typedef struct Token {
    TokenType type;

    const char* identifier;
    size_t size;

    Position pos;

    word value;
} Token;

Token Token_init(const char* identifier, size_t size, TokenType type, word value, Position pos);

// for the list interface:
void Token_print(void* token);
void* Token_copy(void* token);
void Token_destroy(void* token);

typedef struct Lexer {
    const char* content;
    const char* end;
    const char* start;
    const char* file_name;
    Position pos;
} Lexer;

Lexer Lexer_init(const char* content, const char* file_name);
Token Lexer_nextToken(Lexer* lexer);
List* lexer_lexAllContent(Lexer* lexer, List* label_list, List* alias_list);

Token Lexer_lexNumber(Lexer* lexer, char base);
Token Lexer_lexSymbol(Lexer* lexer);
Token Lexer_lexNAME(Lexer* lexer);
Token Lexer_lexComment(Lexer* lexer);
Token Lexer_lexDirective(Lexer* lexer);

#endif // LEXER_H_
