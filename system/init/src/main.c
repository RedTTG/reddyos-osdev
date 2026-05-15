static long sys_test(long val)
{
    long ret;
    __asm__ volatile (
        "mov $1, %%rax\n"
        "mov %1, %%rdi\n"
        "syscall\n"
        "mov %%rax, %0\n"
        : "=r"(ret)
        : "r"(val)
        : "rax", "rcx", "r11"
    );
    return ret;
}

void _start(void)
{
    long i = 0;
    for (;;) {
        i++;
        // sys_test(i);
    }
}