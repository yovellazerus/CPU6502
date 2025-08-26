#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define MAX_TOKENS 256
#define MAX_FUNCTION_VALUES 8

typedef enum {
    Token_EOF,
    Token_ERROR,
    Token_NUMBER,
    Token_NAME,
    Token_PLUS,
    Token_MINOS,
    Token_CURET,
    Token_DIV,
    Token_MUL,
    Token_MOD,
    Token_OPEN,
    Token_CLOSE,
    Token_EXCLAMATION,
} TokenType;

static char* type_to_str_table[] = {
    [Token_EOF] = "EOF",
    [Token_ERROR] = "ERROR",
    [Token_NUMBER] = "NUMBER",
    [Token_NAME] = "NAME",
    [Token_PLUS] = "PLUS",
    [Token_MINOS] = "MINOS",
    [Token_CURET] = "CURET",
    [Token_DIV] = "DIV",
    [Token_MUL] = "MUL",
    [Token_MOD] = "MOD",
    [Token_OPEN]= "OPEN",
    [Token_CLOSE] = "CLOSE", 
    [Token_EXCLAMATION] = "EXCLAMATION",
};

typedef struct Token {
    char* name;
    size_t size;
    double value;
    TokenType type;
} Token;

Token Token_init(char* name, size_t size, TokenType type, double value);
void Token_print(Token* token, FILE* stream);
void token_printAll(Token tokenArr[MAX_TOKENS]);

typedef struct Lexer {
    char* content;
    char* end;
    char* start;
} Lexer;

Lexer Lexer_init(char* content);
Token Lexer_nextToken(Lexer* lexer);
Token Lexer_lexNumber(Lexer* lexer);
Token Lexer_lexName(Lexer* lexer);
Token Lexer_lexOperand(Lexer* lexer);
int lexer_lexAllContent(Lexer* lexer, Token tokenArr[MAX_TOKENS]);

#endif // LEXER_H_