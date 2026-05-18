#include "common.h"

#include "mount.h"

mount_t* root_mount = 0;

int mount_fs(const char* path, vnode_t* new_node) {
    if (!new_node)
        return -1;

    if (!root_mount) {
        // If no root mount exists, only allow mounting at root
        if (!path || strcmp(path, "/") != 0)
            return -1;

        static mount_t m;
        m.name = "mounted";
        m.root = new_node;
        root_mount = &m;
        return 0;
    }

    // Mounting at root
    if (!path || strcmp(path, "/") == 0) {
        root_mount->root = new_node;
        return 0;
    }

    // Strip leading '/'
    const char* p = path;
    if (p[0] == '/') p++;

    // Find parent (for simple single-component paths we mount directly under root)
    const char* slash = strchr(p, '/');
    if (slash) {
        // Support nested mount points by traversing
        // For now, we only support a single-level mount (e.g. /dev)
        // Return error for deeper paths
        return -1;
    }

    // Parent is root
    vnode_t* parent = root_mount->root;
    if (!parent)
        return -1;

    vnode_t** children = parent->internal;
    if (!children)
        return -1;

    // Check if a child with same name exists; replace it
    for (int i = 0; children[i]; i++) {
        if (!strcmp(children[i]->name, p)) {
            children[i] = new_node;
            return 0;
        }
    }

    // Append to first empty slot
    for (int i = 0; i < 64; i++) {
        if (!children[i]) {
            children[i] = new_node;
            return 0;
        }
    }

    return -1; // No space
}