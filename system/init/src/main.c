#include <reddyos/terminal.h>

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	terminal_write("Init started!\n");
	terminal_write("Argc: ");
	terminal_write_u64(argc);
	terminal_write("\nArgv: ");
	for (int i = 0; i < argc; i++) {
		terminal_write(argv[i]);
		terminal_write(" ");
	}
	terminal_write("\n");

	for (;;) {
	}
	return 0;
}
