
#include "list.h"
#include "Lexer.h"

#define MAX_FILE_SIZE (256*256)

int main(int argc, char* argv[]){

    if(argc != 2){
        fprintf(stderr, "Usage: ./asm65 <file.asm>");
        return 1;
    }

    char source[MAX_FILE_SIZE] = {'\0'};
    const char* path = argv[1];
    FILE* input = fopen(path, "r");
    if(!input){
        perror(path);
        return 1;
    }
    fread(source, 1, MAX_FILE_SIZE, input);
    fclose(input);

    List* label_list = List_create(Token_copy, Token_destroy, Token_print, NULL);
    List* alias_list = List_create(Token_copy, Token_destroy, Token_print, NULL);

    Lexer lexer = Lexer_init(source, path);
    List* token_list = lexer_lexAllContent(&lexer, label_list, alias_list);
    List_print(token_list);
    List_print(label_list);
    List_print(alias_list);


    List_destroy(token_list);
    List_destroy(label_list);
    List_destroy(alias_list);
    
    return 0;
}