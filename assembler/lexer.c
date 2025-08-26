#include "Lexer.h"

static bool are_you_mnemonic(const char* identifier, size_t size){
    char* str = malloc(size + 1);
    if(!str){
        return false;
    }
    for(int i = 0; i < size; i++){
        str[i] = identifier[i];
    }
    str[size] = '\0';
    if(strcmp("LDA", str) == 0 ||
    strcmp("LDX", str) == 0 ||
    strcmp("LDY", str) == 0 ||
    strcmp("STA", str) == 0 ||
    strcmp("STX", str) == 0 ||
    strcmp("STY", str) == 0 ||
    strcmp("TAX", str) == 0 ||
    strcmp("TAY", str) == 0 ||
    strcmp("TSX", str) == 0 ||
    strcmp("TXA", str) == 0 ||
    strcmp("TXS", str) == 0 ||
    strcmp("TYA", str) == 0 ||
    strcmp("PHA", str) == 0 ||
    strcmp("PHP", str) == 0 ||
    strcmp("PLA", str) == 0 ||
    strcmp("PLP", str) == 0 ||
    strcmp("AND", str) == 0 ||
    strcmp("EOR", str) == 0 ||
    strcmp("ORA", str) == 0 ||
    strcmp("BIT", str) == 0 ||
    strcmp("ADC", str) == 0 ||
    strcmp("SBC", str) == 0 ||
    strcmp("CMP", str) == 0 ||
    strcmp("CPX", str) == 0 ||
    strcmp("CPY", str) == 0 ||
    strcmp("INC", str) == 0 ||
    strcmp("INX", str) == 0 ||
    strcmp("INY", str) == 0 ||
    strcmp("DEC", str) == 0 ||
    strcmp("DEX", str) == 0 ||
    strcmp("DEY", str) == 0 ||
    strcmp("ASL", str) == 0 ||
    strcmp("LSR", str) == 0 ||
    strcmp("ROL", str) == 0 ||
    strcmp("ROR", str) == 0 ||
    strcmp("JMP", str) == 0 ||
    strcmp("JSR", str) == 0 ||
    strcmp("RTS", str) == 0 ||
    strcmp("BCC", str) == 0 ||
    strcmp("BCS", str) == 0 ||
    strcmp("BEQ", str) == 0 ||
    strcmp("BMI", str) == 0 ||
    strcmp("BNE", str) == 0 ||
    strcmp("BPL", str) == 0 ||
    strcmp("BVC", str) == 0 ||
    strcmp("BVS", str) == 0 ||
    strcmp("CLC", str) == 0 ||
    strcmp("CLD", str) == 0 ||
    strcmp("CLI", str) == 0 ||
    strcmp("CLV", str) == 0 ||
    strcmp("SEC", str) == 0 ||
    strcmp("SED", str) == 0 ||
    strcmp("SEI", str) == 0 ||
    strcmp("BRK", str) == 0 ||
    strcmp("NOP", str) == 0 ||
    strcmp("RTI", str) == 0){
        free(str);
        return true;
    }
    free(str);
    return false;
}

Token Token_init(const char* identifier, size_t size, TokenType type, word value, Position pos){
    Token res;
    res.identifier = identifier;
    res.size = size;
    res.type = type;
    res.value = value;
    res.pos = pos;
    return res;
}

void Token_print(const Token* token, FILE* stream){
    fprintf(stream, "{ %.*s, type = %s, val = 0x%.2x, row = %zu, col = %zu, file = `%s` }\n", 
        (int)token->size, 
        token->identifier, 
        type_to_str_table[token->type], 
        token->value, 
        token->pos.row,
        token->pos.coll,
        token->pos.file_name);
}

void token_printAll(const Token tokenArr[MAX_TOKENS]){
    int i = 0;
    Token curr = tokenArr[i];
    while(curr.type != Token_eof){
        curr = tokenArr[i];
        Token_print(&curr, stdout);
        i++;
    }
}

Lexer Lexer_init(const char* content, const char* file_name){
    Position pos = {0};
    pos.file_name = file_name;
    return (Lexer){.content = content, .end = content, .start = content, .file_name = file_name, .pos = pos};
}

Token Lexer_nextToken(Lexer* lexer){
    // chop white space
    while (isspace(*lexer->end)) {
        if (*lexer->end == '\n') {
            lexer->pos.row++;
            lexer->pos.coll = 0;
        } else {
            lexer->pos.coll++;
        }
        lexer->end++;
    }

    // if '\0' return EOF
    if(*(lexer->end) == '\0'){
        return Token_init(lexer->end, 0, Token_eof, 0.0, lexer->pos);
    }

    lexer->start = lexer->end;
    if(isdigit(*(lexer->end)) || *(lexer->end) == '-'){
        return Lexer_lexNumber(lexer);
    }
    else if (*(lexer->end) == '.'){
        //return Lexer_lexDirective(lexer);
    }
    else if (isalpha(*(lexer->end)) || *(lexer->end) == '_'){
        return Lexer_lexNAME(lexer);
    }
    else{
        return Lexer_lexSymbol(lexer);
    }
    return Token_init(0, 0, Token_err, 0, lexer->pos);
    
}

Token Lexer_lexNumber(Lexer* lexer){
    const char* end = NULL;
    word value = 0;
    value = strtod(lexer->end, &end);
    lexer->end = end;
    Token res = Token_init(lexer->start, end - lexer->start, Token_number, value, lexer->pos);
    lexer->pos.coll += (end - lexer->start);
    lexer->start = lexer->end;
    return res;
}

Token Lexer_lexNAME(Lexer *lexer)
{
    Token res = {0};
    while((isalpha(*(lexer->end)) || isdigit(*(lexer->end)) || *(lexer->end) == '_') && *(lexer->end) != '\0'){
        lexer->end++;
    }
    TokenType type = 0;
    bool is_mnemonic = false;
    is_mnemonic = are_you_mnemonic(lexer->start, lexer->end - lexer->start);
    if(is_mnemonic){
        res = Token_init(lexer->start, lexer->end - lexer->start, Token_mnemonic, 0, lexer->pos);
        lexer->start = lexer->end;
    }
    else if(*(lexer->end) == ':'){
        res = Token_init(lexer->start, lexer->end - lexer->start, Token_label, 0, lexer->pos);
        lexer->start = lexer->end;
        lexer->end++;
        lexer->pos.coll++;
    }
    else{
        res = Token_init(lexer->start, lexer->end - lexer->start, Token_alias, 0, lexer->pos);
        lexer->start = lexer->end;
    }
    lexer->pos.coll += lexer->end - lexer->start;
    return res;
}

Token Lexer_lexSymbol(Lexer* lexer){
    Token res = Token_init(NULL, 0, Token_err, 0, lexer->pos);
    char char_op = *(lexer->end); 
    lexer->end++;
    lexer->start++;
    lexer->pos.coll++;
    switch (char_op){
        
        case '(':
            res = Token_init(lexer->end - 1, 1, Token_open, 0, lexer->pos);
            break;
        case ')':
            res = Token_init(lexer->end - 1, 1, Token_close, 0, lexer->pos);
            break;
        case '=':
            res = Token_init(lexer->end - 1, 1, Token_eq, 0, lexer->pos);
            break;
        case '#':
            res = Token_init(lexer->end - 1, 1, Token_hash, 0, lexer->pos);
            break;

        default:
            res = Token_init(lexer->end - 1, 1, Token_err, 0, lexer->pos);
            break;
    }
    return res;
}

int lexer_lexAllContent(Lexer* lexer, Token tokenArr[MAX_TOKENS]){
    Token curr = Token_init(NULL, 0, Token_err, 0.0, (Position){0});
    int i = 0;
    while(curr.type != Token_eof){
        curr = Lexer_nextToken(lexer);
        tokenArr[i] = curr;
        i++;
    }
    return i;
}