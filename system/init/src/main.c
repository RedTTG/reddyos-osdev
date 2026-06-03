#include <reddyos/terminal.h>
#include <reddyos/fb.h>
#include <stdlib.h>
#include "stdio.h"
#include <string.h>
#include <sys/ioctl.h>

extern char **environ;

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
	terminal_write("Successfully opened /dev/fb0\n");

	int fd = fileno(fb);  // key step
	terminal_write("File descriptor for /dev/fb0: ");
	terminal_write_u64(fd);
	terminal_write("\n");

	fb_info_t* info = malloc(sizeof(fb_info_t));
	if (!info)
		return 1;
	memset(info, 0, sizeof(fb_info_t));

	// run IOctl

	ioctl(fd, FB_IOCTL_GET_INFO, info);

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
	terminal_write("\n\n");

	for (size_t y = 0; y < info->height; y++) {
		for (size_t x = 0; x < info->width; x++) {
			uint32_t nX = x * 255 / info->width;
			uint32_t nY = y * 255 / info->height;
			fseek(fb, (y * (info->pitch / 4) + x), SEEK_SET);
			uint32_t pixel = (nY << 8) | nX;
			fwrite(&pixel, sizeof(pixel), 1, fb);
		}
	}

	ioctl(fd, FB_IOCTL_FLIP, NULL);
	terminal_write("Flipped!\n");

	free(info);

	for (;;) {
		// terminal_putc('X');
	}
	return 0;
}
