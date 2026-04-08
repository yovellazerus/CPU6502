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

typedef uint8_t byte;
typedef uint16_t word;

#define UNUSED() do                                                             \
{                                                                               \
    fprintf(stderr, "UNUSED: function: \"%s\" in file: \"%s\" is unused.\n",    \
                                        __func__, __FILE__);                    \
    exit(1);                                                                    \
} while (0);

#define ERROR(cpu, msg, ...) CPU_error(cpu, msg, ##__ARGS__); \
                            fprintf(stderr, "in: (%s:%s:%d)", \
                                    __FILE__, __func__, __LINE__)

#endif // COMMON_H_