static long sys_test(long val)
{
    long ret;
    __asm__ volatile (
        "syscall\n"
        : "=a"(ret)           // Output: RAX contains return value
        : "a"(1),             // RAX = syscall number (1)
          "D"(val)            // RDI = argument (val)
        : "rcx", "r11"        // Clobbered by syscall
    );
    return ret;
}

void _start(void)
{
    long i = 1;
    for (;;) {
        i++;
        i = sys_test(i);
    }
}