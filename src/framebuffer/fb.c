#include "common.h"

reddyos_framebuffer_t* fb0 = NULL;

void fb_init(void) {
    struct limine_framebuffer* limine_fb = framebuffer_request.response->framebuffers[0];
    if (!limine_fb->address) {
        panic("No framebuffer address provided by bootloader");
    }

    fb0 = kmalloc(sizeof(reddyos_framebuffer_t));
    if (!fb0) {
        panic("Failed to allocate memory for framebuffer");
    }

    memset(fb0, 0, sizeof(reddyos_framebuffer_t));

    fb0->front = limine_fb->address;

    fb0->width  = limine_fb->width;
    fb0->height = limine_fb->height;
    fb0->pitch  = limine_fb->pitch;
    fb0->bpp    = limine_fb->bpp;

    fb0->size = fb0->pitch * fb0->height;

    fb0->back = kmalloc(fb0->size);
    memset(fb0->back, 0, fb0->size);
}

static int fb_read(vnode_t *vnode, uint64_t offset, void *buffer, uint64_t size) {
    reddyos_framebuffer_t* fb = (reddyos_framebuffer_t*)vnode->internal;

    if (offset >= fb->size)
        return 0;

    if (offset + size > fb->size)
        size = fb->size - offset;

    memcpy(buffer, ((uint8_t*)fb->back) + offset, size);

    return (int)size;
}

static int fb_write(vnode_t *vnode, uint64_t offset, const void *buffer, uint64_t size) {
    reddyos_framebuffer_t* fb = (reddyos_framebuffer_t*)vnode->internal;

    if (offset >= fb->size)
        return 0;

    if (offset + size > fb->size)
        size = fb->size - offset;

    memcpy(((uint8_t*)fb->back) + offset, buffer, size);

    return (int)size;
}

static vnode_ops_t fb_ops =
{
    .read = fb_read,
    .write = fb_write
};

void fb_device_init(void) {
    if (!fb0) {
        fb_init();
    }

    devfs_register("fb0", &fb_ops, fb0);
}
