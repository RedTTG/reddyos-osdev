#pragma once

#define FB_OP_CLEAR 1
#define FB_OP_FLIP 2
#define FB_NOP 0

typedef struct reddyos_framebuffer {
    uint32_t* front;
    uint32_t* back;

    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;

    size_t size;
    uint8_t clear_color;
    uint8_t op;
} reddyos_framebuffer_t;

void fb_init(void);
void fb_device_init(void);

extern reddyos_framebuffer_t* fb0;