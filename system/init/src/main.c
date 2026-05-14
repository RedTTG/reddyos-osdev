#include <stdint.h>

// Very small init program — for now just loop. Kernel will ELF-load this and
// can interact with it later.

int main(void) {
    while (1) {
        __asm__ volatile ("hlt");
    }
    return 0;
}

