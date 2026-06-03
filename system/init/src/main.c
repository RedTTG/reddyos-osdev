#include <reddyos/terminal.h>
#include <reddyos/fb.h>
#include <stdlib.h>
#include "stdio.h"
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>

#include "stdbool.h"
#include "abi-bits/vm-flags.h"

#define FRAME_NS 16666666ULL

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;


	FRAMEBUFFER* fb = fb_get("/dev/fb0");
	if (!fb) {
		perror("fb_get");
	}

	bool go_x = true;
	bool go_y = true;
	int x = 5, y = 5;

	struct timespec start, end;

	while (1) {
		// clock_gettime(CLOCK_MONOTONIC, &start);
		//
		fb_clear(fb, 0x00000000);
		fb_draw_rect(fb, x, y, 50, 50, 0x00FF0000, 5);
		fb_draw_rect(fb, x, y, 50, 50, 0x000000FF, 0);
		fb_flip(fb);
		//
		// clock_gettime(CLOCK_MONOTONIC, &end);
		// long elapsed_ns =
		// (end.tv_sec - start.tv_sec) * 1000000000L +
		// (end.tv_nsec - start.tv_nsec);
		//
		// if (elapsed_ns < FRAME_NS) {
		// 	struct timespec sleep = {
		// 		.tv_sec = 0,
		// 		.tv_nsec = FRAME_NS - elapsed_ns
		// 	};
		//
		// 	nanosleep(&sleep, NULL);
		// }

	}

	fb_close(fb);

	for (;;) {
		// terminal_putc('X');
	}
	return 0;
}
