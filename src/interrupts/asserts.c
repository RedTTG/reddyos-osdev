#include "common.h"

void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function) {
    __asm__ volatile ("sti");
    terminal_write("Assertion Failed: ");
    terminal_write(assertion);
    terminal_write("\nFile: ");
    terminal_write(file);
    terminal_write("\nLine: ");
    terminal_write_u64(line);
    terminal_write("\nFunction: ");
    terminal_write(function);
    terminal_write("\n");
    panic("Assertion Failed");
}