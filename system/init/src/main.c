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

#define FPS 120
#define FRAME_NS (1000000000L / FPS)
#define MOVE 5
#define SQUARE_WIDTH 100
#define SQUARE_HEIGHT 100

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
		clock_gettime(CLOCK_MONOTONIC, &start);
		//
		// fb_clear(fb, 0x00101010);
		fb_draw_rect(fb, 0, 0, fb->info.width, fb->info.height, 0x00FF0000, 3);
		fb_draw_rect(fb, x, y, SQUARE_WIDTH, SQUARE_HEIGHT, 0x00FF0000, 5);
		fb_draw_rect(fb, x, y, SQUARE_WIDTH, SQUARE_HEIGHT, 0x000000FF, 0);
		fb_flip(fb);

		x += go_x ? MOVE : -MOVE;
		y += go_y ? MOVE : -MOVE;
		if (x+SQUARE_WIDTH >= fb->info.width || x <= 0) {
			go_x = !go_x;
		}
		if (y+SQUARE_HEIGHT >= fb->info.height || y <= 0) {
			go_y = !go_y;
		}


		//
		clock_gettime(CLOCK_MONOTONIC, &end);
		long elapsed_ns =
		(end.tv_sec - start.tv_sec) * 1000000000L +
		(end.tv_nsec - start.tv_nsec);

		if (elapsed_ns < FRAME_NS) {
			struct timespec sleep = {
				.tv_sec = 0,
				.tv_nsec = FRAME_NS - elapsed_ns
			};

			nanosleep(&sleep, NULL);
		}

	}

	fb_close(fb);

	for (;;) {
		// terminal_putc('X');
	}
	return 0;
}
