#ifndef LEXER_H_
#define LEXER_H_

#include <ctype.h>
#include <string.h>

#include "list.h"
#include "..\include\utilities.h"
#include "..\include\cpu.h"

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
