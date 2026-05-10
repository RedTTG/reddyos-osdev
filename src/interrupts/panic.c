#include "common.h"
void panic(const char* msg)
{
    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);
    terminal_write("\n");

    __asm__ volatile("cli");
    while (1)
        __asm__ volatile("hlt");
}