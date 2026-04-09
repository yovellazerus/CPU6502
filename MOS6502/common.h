#ifndef COMMON_H_
#define COMMON_H_

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

typedef uint8_t byte;
typedef uint16_t word;

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

#endif // COMMON_H_