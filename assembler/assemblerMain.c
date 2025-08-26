
#include "list.h"
#include "Lexer.h"

#define MAX_FILE_SIZE (256*256)

void* double_copy(void* x){
    double* res = malloc(sizeof(double));
    *res = *((double*)x);
    return res;
}

void double_print(void* x){
    printf("%lf", *((double*)x));
}

void double_destroy(void* x){
    free(x);
}

int main(int argc, char* argv[]){

    double x[] = {0, 1, 2, 3, 4};
    List* a = List_create(double_copy, double_destroy, double_print, NULL);
    List_pushBack(a, &x[0]);
    List_pushBack(a, &x[1]);
    List_pushBack(a, &x[2]);
    List_pushBack(a, &x[3]);
    List_pushBack(a, &x[4]);
    List_print(a);
    List_destroy(a);

    char source[MAX_FILE_SIZE] = {'\0'};
    const char* path = "../input/basic.asm";
    FILE* input = fopen(path, "r");
    if(!input){
        perror(path);
        return 1;
    }

    fread(source, 1, MAX_FILE_SIZE, input);

    Token tokens[MAX_TOKENS];
    Lexer lexer = Lexer_init(source);

    lexer_lexAllContent(&lexer, tokens);

    token_printAll(tokens);

    fclose(input);

    return 0;
}