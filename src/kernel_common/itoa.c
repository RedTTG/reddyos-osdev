#include "common.h"
static const char* digits = "0123456789abcdef";

void itoa(uint64_t value, char* buffer, int base)
{
    char tmp[65];
    int i = 0;

    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = 0;
        return;
    }

    while (value)
    {
        tmp[i++] = digits[value % base];
        value /= base;
    }

    int j = 0;
    while (i--)
        buffer[j++] = tmp[i];

    buffer[j] = 0;
}