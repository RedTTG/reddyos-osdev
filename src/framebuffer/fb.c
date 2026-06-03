#include "common.h"

reddyos_framebuffer_t *fb0 = NULL;

void fb_init(void) {
    struct limine_framebuffer *limine_fb = framebuffer_request.response->framebuffers[0];
    if (!limine_fb->address) {
        panic("No framebuffer address provided by bootloader");
    }

    fb0 = kmalloc(sizeof(reddyos_framebuffer_t));
    if (!fb0) {
        panic("Failed to allocate memory for framebuffer");
    }

    memset(fb0, 0, sizeof(reddyos_framebuffer_t));

    fb0->front = limine_fb->address;

    fb0->width = limine_fb->width;
    fb0->height = limine_fb->height;
    fb0->pitch = limine_fb->pitch;
    fb0->bpp = limine_fb->bpp;

    fb0->size = fb0->pitch * fb0->height;

    fb0->back = kmalloc(fb0->size);
    memset(fb0->back, 0, fb0->size);
}

static int fb_read(vnode_t *vnode, uint64_t offset, void *buffer, u64 size) {
    reddyos_framebuffer_t *fb = (reddyos_framebuffer_t *) vnode->internal;

    if (offset >= fb->size)
        return 0;

    if (offset + size > fb->size)
        size = fb->size - offset;

    memcpy(buffer, ((u8 *) fb->back) + offset, size);

    return (int) size;
}

static int fb_write(vnode_t *vnode, uint64_t offset, const void *buffer, u64 size) {
    // terminal_write("fb_write called with offset ");
    // terminal_write_u64(offset);
    // terminal_write(" and size ");
    // terminal_write_u64(size);
    // terminal_write("\n");
    reddyos_framebuffer_t *fb = (reddyos_framebuffer_t *) vnode->internal;

    if (offset >= fb->size)
        return 0;

    if (offset + size > fb->size)
        size = fb->size - offset;

    memcpy(((u32 *) fb->back) + offset, buffer, size);

    return (int) size;
}

static int fb_ioctl(vnode_t *vnode, uint64_t cmd, u64 arg) {
    reddyos_framebuffer_t *fb = (reddyos_framebuffer_t *) vnode->internal;

    switch (cmd) {
        case FB_IOCTL_FLIP:
            if (fb)
                memcpy(fb->front, fb->back, fb->size);
            return 0;

        case FB_IOCTL_CLEAR: {
            uint8_t color = (uint8_t) arg;
            if (fb)
                memset(fb->back, color, fb->size);
            return 0;
        }

        case FB_IOCTL_GET_INFO: {
            fb_info_t *info = (fb_info_t *) arg;
            if (!info || !fb)
                return -1;
            info->width = fb->width;
            info->height = fb->height;
            info->pitch = fb->pitch;
            info->bpp = fb->bpp;
            info->size = fb->size;
            return 0;
        }

        default:
            return -1; // Unknown request
    }
}

static bool fb_vma_fault(vnode_t *vnode, vm_area_t *vma, u64 page) {
    reddyos_framebuffer_t *fb = (reddyos_framebuffer_t *) vnode->internal;

    if (!fb)
        return false;

    u64 offset = (page - vma->start) + vma->pgoff;
    if (offset >= fb->size)
        return false;

    // Map the requested page to the framebuffer memory
    vmm_map(&CUR_PROCESS->address_space, page, (u64)vmm_lookup(((u64) fb->back) + offset), PAGE_USER | PAGE_WRITABLE);
    return true;
}

static vnode_ops_t fb_ops =
{
    .read = fb_read,
    .write = fb_write,
    .ioctl = fb_ioctl,
    .vma_fault = fb_vma_fault
};

void fb_device_init(void) {
    if (!fb0) {
        fb_init();
    }

    vnode_t *node = devfs_register("fb0", &fb_ops, fb0);
    if (node) {
        node->size = fb0->size;
    }
}
