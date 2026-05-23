static inline void term_putc(char c) {
	register long rax asm("rax") = 100;  /* sys_term */
	register long rdi asm("rdi") = c;
	asm volatile("syscall" : "+r"(rax) : "r"(rdi) : "rcx", "r11", "memory");
}

static void term_puts(const char *str) {
	while (*str) {
		term_putc(*str);
		str++;
	}
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	/* Try terminal output to verify init is running */
	term_puts("Init started!\n");

	for (;;) {
		term_putc('.');
	}
	return 0;
}
