#include "common.h"
#include "fs_flags.h"

void* vfs_find_vnode(const char* path) {
    if (!root_mount)
        return NULL;

    vnode_t* node = root_mount->root;

    // Handle paths starting with /
    if (path[0] == '/')
        path++;

    // Handle empty path (root)
    if (path[0] == '\0')
        return node;

    // Strip trailing slash
    size_t len = strlen(path);
    char tmp[128];
    if (len >= sizeof(tmp))
        return NULL;
    strcpy(tmp, path);
    if (len > 0 && tmp[len-1] == '/')
        tmp[len-1] = '\0';

    // First, check if there's a direct child with the full name
    // (This handles tarfs files like "bin/init" stored as single names)
    vnode_t** children = node->internal;
    if (children) {
        for (int i = 0; children[i]; i++) {
            if (!strcmp(children[i]->name, tmp)) {
                return children[i];
            }
        }
    }

    // If no direct match, traverse path component by component
    // (This handles the new mount point structure like /dev/fb0)
    char* path_copy = tmp;
    while (*path_copy) {
        // Find the next '/' or end of string
        char* slash = strchr(path_copy, '/');
        size_t component_len;

        if (slash) {
            component_len = slash - path_copy;
        } else {
            component_len = strlen(path_copy);
        }

        if (component_len == 0)
            return NULL; // Empty component

        // Extract the component name
        char component[64];
        if (component_len >= sizeof(component))
            return NULL;
        strncpy(component, path_copy, component_len);
        component[component_len] = '\0';

        // Search children of current node
        children = node->internal;
        if (!children)
            return NULL;

        vnode_t* found = NULL;
        for (int i = 0; children[i]; i++) {
            if (!strcmp(children[i]->name, component)) {
                found = children[i];
                break;
            }
        }

        if (!found)
            return NULL;

        node = found;

        // Move to next component
        if (slash) {
            path_copy = slash + 1;
        } else {
            break; // No more components
        }
    }

    return node;
}

int vfs_is_dir(const char* path) {
    if (!root_mount) return 0;
    vnode_t* node = vfs_find_vnode(path);
    if (!node) return 0;
    // Treat a vnode as directory when ops is NULL and internal is non-NULL
    return (node->ops == NULL && node->internal != NULL) ? 1 : 0;
}

int vfs_dir_count(const char* path) {
    vnode_t* node = vfs_find_vnode(path);
    if (!node) return -1;
    vnode_t** children = node->internal;
    if (!children) return -1;
    int count = 0;
    for (int i = 0; children[i]; i++) count++;
    return count;
}

int vfs_open(const char* path, file_t* out, int flags, int mode)
{
    if (!root_mount)
        return -1;

    vnode_t* node = vfs_find_vnode(path);
    if (!node)
        return -1;

    out->vnode = node;
    out->offset = 0;
    out->refcount = 0;
    out->flags = flags;

    /* Basic handling for O_TRUNC: adjust vnode size and reset offset.
     * Proper truncation should be implemented by the filesystem's
     * vnode_ops (if supported). This is a minimal behavior so that
     * callers relying on truncation don't see stale size/offset. */
    if (flags & O_TRUNC) {
        out->vnode->size = 0;
        out->offset = 0;
    }

    return 0;
}

ssize_t vfs_read(
    file_t *file,
    void *buffer,
    uint64_t size
)
{
    int read = file->vnode->ops->read(
        file->vnode,
        file->offset,
        buffer,
        size
    );

    if (read > 0)
        file->offset += read;

    return read;
}

ssize_t vfs_write(
    file_t *file,
    const void *buffer,
    uint64_t size
)
{
    int write = file->vnode->ops->write(
        file->vnode,
        file->offset,
        buffer,
        size
    );

    if (write > 0)
        file->offset += write;

    return write;
}

int vfs_ioctl(file_t *file, uint64_t cmd, void *arg)
{
    if (!file || !file->vnode || !file->vnode->ops)
        return -1;

    if (!file->vnode->ops->ioctl)
        return -1;  // Operation not supported

    return file->vnode->ops->ioctl(file->vnode, cmd, arg);
}

off_t vfs_lseek(file_t *file, off_t offset, int whence)
{
    if (!file || !file->vnode)
        return -1;

    off_t new_offset = 0;

    switch (whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = (off_t)file->offset + offset;
            break;
        case SEEK_END:
            new_offset = (off_t)file->vnode->size + offset;
            break;
        default:
            return -1;  // Invalid whence
    }

    // Clamp to valid range [0, vnode->size]
    if (new_offset < 0)
        new_offset = 0;
    if (new_offset > (off_t)file->vnode->size)
        new_offset = (off_t)file->vnode->size;

    file->offset = (uint64_t)new_offset;
    return new_offset;
}
