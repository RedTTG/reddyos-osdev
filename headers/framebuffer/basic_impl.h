
#include <stddef.h>
#include <stdint.h>
struct limine_framebuffer;
static volatile uint32_t* fb_ptr;
static size_t fb_stride;
static size_t fb_width;
static size_t fb_height;

static int square_x;
static int square_y;
static int square_dx = 4;
static int square_dy = 2;
static uint32_t square_size = 32;
static uint32_t square_ticks = 0;

static const uint32_t FB_BLACK = 0x000000;
static const uint32_t FB_RED   = 0xFF0000;

static void fb_put_pixel(size_t x, size_t y, uint32_t color)
{
    if (x >= fb_width || y >= fb_height)
        return;

    fb_ptr[y * fb_stride + x] = color;
}

static void fb_fill(uint32_t color)
{
    for (size_t y = 0; y < fb_height; y++) {
        for (size_t x = 0; x < fb_width; x++) {
            fb_put_pixel(x, y, color);
        }
    }
}

static void fb_draw_square(int x, int y, uint32_t color)
{
    for (uint32_t yy = 0; yy < square_size; yy++) {
        for (uint32_t xx = 0; xx < square_size; xx++) {
            fb_put_pixel((size_t)(x + (int)xx), (size_t)(y + (int)yy), color);
        }
    }
}

void animate_square(void)
{
    square_ticks++;
    if (square_ticks < 1000 * 1000)
        return;
    square_ticks = 0;
    fb_draw_square(square_x, square_y, FB_BLACK);

    int next_x = square_x + square_dx;
    int next_y = square_y + square_dy;

    if (next_x < 0 || (uint32_t)next_x + square_size >= fb_width) {
        square_dx = -square_dx;
        next_x = square_x + square_dx;
    }

    if (next_y < 0 || (uint32_t)next_y + square_size >= fb_height) {
        square_dy = -square_dy;
        next_y = square_y + square_dy;
    }

    if (next_x < 0)
        next_x = 0;
    if (next_y < 0)
        next_y = 0;

    square_x = next_x;
    square_y = next_y;

    fb_draw_square(square_x, square_y, FB_RED);
}
void square_init(struct limine_framebuffer *framebuffer) {
    fb_ptr = framebuffer->address;
    fb_stride = framebuffer->pitch / sizeof(uint32_t);
    fb_width = framebuffer->width;
    fb_height = framebuffer->height;
    square_size = (uint32_t)((fb_width < fb_height ? fb_width : fb_height) / 8);
    if (square_size < 8)
        square_size = 8;
    if (square_size > 64)
        square_size = 64;

    // Start with a black screen and an initial red square.
    fb_fill(FB_BLACK);
    square_x = 0;
    square_y = 0;
    fb_draw_square(square_x, square_y, FB_RED);
}