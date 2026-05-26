#include <reddyos/terminal.h>
#include <reddyos/fb.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "stdio.h"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	terminal_write("Opening /dev/fb0\n");
	FILE* fb = fopen("/dev/fb0", "r+b");
	if (!fb) {
		terminal_write("Failed to open /dev/fb0\n");
		perror("fopen");
		return 1;
	}

	int fd = fileno(fb);  // key step

	fb_info_t* info = malloc(sizeof(fb_info_t));

	if (ioctl(fd, FB_IOCTL_GET_INFO, info) < 0) {
		perror("ioctl");
		return 1;
	}

	terminal_write("Framebuffer info:\n");
	terminal_write("Width: ");
	terminal_write_u64(info->width);
	terminal_write("\nHeight: ");
	terminal_write_u64(info->height);
	terminal_write("\nPitch: ");
	terminal_write_u64(info->pitch);
	terminal_write("\nBPP: ");
	terminal_write_u64(info->bpp);
	terminal_write("\nSize: ");
	terminal_write_u64(info->size);
	terminal_write("\n");

	free(info);

	for (;;) {

	}
	return 0;
}
