#include "common.h"

int vfs_open(const char* path, file_t* out)
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

            return 0;
        }
    }

    return -1;
}

int vfs_read(
    file_t* file,
    void* buffer,
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