#include "Lexer.h"

Token Token_init(char* name, size_t size, TokenType type, double value){
    Token res;
    res.name = name;
    res.size = size;
    res.type = type;
    res.value = value;
    return res;
}

void Token_print(Token* token, FILE* stream){
    fprintf(stream, "{ %.*s, %s, %.2lf }\n", (int)token->size, token->name, type_to_str_table[token->type], token->value);
}

void token_printAll(Token tokenArr[MAX_TOKENS]){
    int i = 0;
    Token curr = tokenArr[i];
    while(curr.type != Token_EOF){
        curr = tokenArr[i];
        Token_print(&curr, stdout);
        i++;
    }
}

Lexer Lexer_init(char* content){
    return (Lexer){.content = content, .end = content, .start = content};
}

Token Lexer_nextToken(Lexer* lexer){
    // chop white space
    while(isspace(*(lexer->end)) && *(lexer->end) != '\0'){
        lexer->end++;
    }
    // if '\0' return EOF
    if(*(lexer->end) == '\0'){
        return Token_init(lexer->end, 0, Token_EOF, 0.0);
    }

    // start for new token
    lexer->start = lexer->end;
    if(isdigit(*(lexer->end)) 
        // for (negative numbers)
        || (*(lexer->end) == '-' && *(lexer->end + 1) != '\0' && isdigit(*(lexer->end + 1)))){
        return Lexer_lexNumber(lexer);
    }
    else if (isalpha(*(lexer->end))){
        return Lexer_lexName(lexer);
    }
    else{
        return Lexer_lexOperand(lexer);
    }
}

Token Lexer_lexNumber(Lexer* lexer){
    char* end = NULL;
    double value = 0.0;
    value = strtod(lexer->end, &end);
    lexer->end = end;
    Token res = Token_init(lexer->start, end - lexer->start, Token_NUMBER, value);
    lexer->start = lexer->end;
    return res;
}

Token Lexer_lexName(Lexer* lexer){
    while((isalpha(*(lexer->end)) || isdigit(*(lexer->end)) || *(lexer->end) == '_') && *(lexer->end) != '\0'){
        lexer->end++;
    }
    Token res = Token_init(lexer->start, lexer->end - lexer->start, Token_NAME, 0.0);
    lexer->start = lexer->end;
    return res;
}

Token Lexer_lexOperand(Lexer* lexer){
    Token res = Token_init(NULL, 0, Token_EOF, 0.0);
    // one char op only for now..
    char char_op = *(lexer->end); 
    lexer->end++;
    lexer->start++;
    switch (char_op){
        case '+':
            res = Token_init(lexer->end - 1, 1, Token_PLUS, 0.0);
            break;
        case '-':
            res = Token_init(lexer->end - 1, 1, Token_MINOS, 0.0);
            break;
        case '*':
            res = Token_init(lexer->end - 1, 1, Token_MUL, 0.0);
            break;
        case '/':
            res = Token_init(lexer->end - 1, 1, Token_DIV, 0.0);
            break;
        case '%':
            res = Token_init(lexer->end - 1, 1, Token_MOD, 0.0);
            break;
        case '^':
            res = Token_init(lexer->end - 1, 1, Token_CURET, 0.0);
            break;
        case '(':
            res = Token_init(lexer->end - 1, 1, Token_OPEN, 0.0);
            break;
        case ')':
            res = Token_init(lexer->end - 1, 1, Token_CLOSE, 0.0);
            break;
        case '!':
            res = Token_init(lexer->end - 1, 1, Token_EXCLAMATION, 0.0);
            break;
        default:
            res = Token_init(lexer->end - 1, 1, Token_ERROR, 0.0);
            break;
    }
    return res;
}

int lexer_lexAllContent(Lexer* lexer, Token tokenArr[MAX_TOKENS]){
    Token curr = Token_init(NULL, 0, Token_ERROR, 0.0);
    int i = 0;
    while(curr.type != Token_EOF){
        curr = Lexer_nextToken(lexer);
        tokenArr[i] = curr;
        i++;
    }
    return i;
}