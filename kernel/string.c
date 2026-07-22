
#include "comman.h"

char* strcpy(char *s, const char *t){
    char *os;

    os = s;
    while ((*s++ = *t++) != 0)
        ;
    return os;
}

int strcmp(const char *p, const char *q){
    while (*p && *p == *q)
        p++, q++;
    return (uint8_t)*p - (uint8_t)*q;
}

uint16_t strlen(const char *s)
{
    int n;

    for (n = 0; s[n] && n < (uint16_t)-1; n++)
        ;
    return n;
}

char* strchr(const char* s, char c)
{
    for (; *s; s++)
        if (*s == c)
        return (char *)s;
    return 0;
}

int atoi(const char *s)
{
    int n;
    
    n = 0;
    while ('0' <= *s && *s <= '9')
      n = n * 10 + *s++ - '0';
    return n;
}

void* memmove(void *vdst, const void *vsrc, int n)
{
    char *dst;
    const char *src;

    dst = vdst;
    src = vsrc;
    if (src > dst) {
        while (n-- > 0)
        *dst++ = *src++;
    } else {
        dst += n;
        src += n;
        while (n-- > 0)
        *--dst = *--src;
    }
    return vdst;
}

int memcmp(const void* s1, const void* s2, uint16_t size)
{
    const char *p1 = s1, *p2 = s2;
    while (size-- > 0) {
        if (*p1 != *p2) {
        return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

void* memcpy(void* dst, const void* src, uint16_t size)
{
    return memmove(dst, src, size);
}