#include "c_generic/string.h"

size_t strlen(const char* str)
{
    size_t len = 0;

    while (str[len])
        len++;

    return len;
}

char* strcpy(char* dest, const char* src)
{
    char* out = dest;

    while ((*dest++ = *src++))
        ;

    return out;
}

int strcmp(const char* a, const char* b)
{
    while (*a && (*a == *b))
    {
        a++;
        b++;
    }

    return *(unsigned char*)a -
           *(unsigned char*)b;
}