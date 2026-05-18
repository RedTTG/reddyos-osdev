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

char* strchr(const char* s, int c)
{
    char ch = (char)c;
    while (*s) {
        if (*s == ch)
            return (char*)s;
        s++;
    }
    if (ch == '\0')
        return (char*)s; // point to NUL terminator
    return NULL;
}

char* strncpy(char* dest, const char* src, size_t n)
{
    char* d = dest;
    size_t i = 0;

    // Copy up to n bytes or until src ends
    for (; i < n && src[i] != '\0'; i++) {
        d[i] = src[i];
    }

    // If we hit end of src before n, pad with NULs
    for (; i < n; i++) {
        d[i] = '\0';
    }

    return dest;
}
