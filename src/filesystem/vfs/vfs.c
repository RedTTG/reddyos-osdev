#include "common.h"

int vfs_open(const char* path, file_t* out, int flags, int mode)
{
    if (!root_mount)
        return -1;

    vnode_t* node = root_mount->root;

    vnode_t** children = node->internal;

    // Handle paths starting with /
    if (path[0] == '/')
        path++;

    for (int i = 0; children[i]; i++)
    {
        if (!strcmp(children[i]->name, path))
        {
            out->vnode = children[i];
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
    }

    return -1;
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