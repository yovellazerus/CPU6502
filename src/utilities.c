
#include "../include\utilities.h"

void set_color(Color color, FILE* file){
    if(!file){
        file = stdout;
    }
    switch (color) {
        case COLOR_RED:     fprintf(file, "\033[0;31m"); break;
        case COLOR_GREEN:   fprintf(file, "\033[0;32m"); break;
        case COLOR_YELLOW:  fprintf(file, "\033[0;33m"); break;
        case COLOR_BLUE:    fprintf(file, "\033[0;34m"); break;
        case COLOR_MAGENTA: fprintf(file, "\033[0;35m"); break;
        case COLOR_CYAN:    fprintf(file, "\033[0;36m"); break;
        case COLOR_WHITE:   fprintf(file, "\033[0;37m"); break;
        case COLOR_RESET:
        default:            fprintf(file, "\033[0m");    break;
    }
}