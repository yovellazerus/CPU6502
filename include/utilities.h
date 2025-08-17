#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define IS_SIGN_BYTE(ARG) ((ARG & 0x80) != 0)


#define MEMORY_SIZE (256*256)

#define STACK_START 0x0100
#define STACK_END 0x01ff

typedef uint8_t byte;
typedef uint16_t word;

typedef enum {
    COLOR_RESET,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
} Color;

void set_color(Color color, FILE* file);

#endif // UTILITIES_H_
