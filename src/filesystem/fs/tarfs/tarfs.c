#include "common.h"
#include "fs_flags.h"

typedef struct tar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
} tar_header_t;

static int tar_read(
    vnode_t *vnode,
    uint64_t offset,
    void *buffer,
    uint64_t size
) {
    uint8_t *data = vnode->internal;

    if (offset >= vnode->size)
        return 0;

    if (offset + size > vnode->size)
        size = vnode->size - offset;

    memcpy(buffer, data + offset, size);

    return (int)size;
}

static int tar_stat(vnode_t *vnode, struct stat *buffer) {
    if (!vnode || !buffer)
        return -1;

    memset(buffer, 0, sizeof(struct stat));

    buffer->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;
    buffer->st_size = vnode->size;

    return 0;
}

static vnode_ops_t tar_ops =
{
    .read = tar_read,
    .write = NULL,
    .ioctl = NULL,
    .vma_fault = NULL
};

static vnode_t root_node;

static vnode_t *children[64];

static uint64_t oct2bin(const char *str) {
    uint64_t value = 0;

    while (*str) {
        value *= 8;
        value += *str - '0';

        str++;
    }

    return value;
}

void tarfs_init(void *address) {
    uint8_t *ptr = address;

    int child_count = 0;

    while (1) {
        tar_header_t *hdr = (tar_header_t *) ptr;

        if (!hdr->name[0])
            break;

        uint64_t size = oct2bin(hdr->size);

        static vnode_t nodes[64];

        vnode_t* node = &nodes[child_count];

        memset(node, 0, sizeof(vnode_t));

        // Fix issue with tar having root "." folder
        const char* name = hdr->name;

        if (name[0] == '.' && name[1] == '/')
            name += 2;

        strcpy(node->name, name);
        // terminal_write("Found file: ");
        // terminal_write(node->name);
        // terminal_write("\n");

        node->size = size;
        node->ops = &tar_ops;

        node->internal = ptr + 512;

        children[child_count++] = node;

        uint64_t full =
                ((size + 511) / 512) * 512;

        ptr += 512 + full;
    }

    children[child_count] = 0;

    memset(&root_node, 0, sizeof(root_node));

    strcpy(root_node.name, "/");

    root_node.internal = children;

    static mount_t root_mount_obj;

    root_mount_obj.name = "tarfs";
    root_mount_obj.root = &root_node;

    root_mount = &root_mount_obj;
}

void tarsf_limine_init() {
    const struct limine_module_response *modules =
            module_request.response;

    const struct limine_file *mod =
            modules->modules[0];

    tarfs_init(mod->address);
}
