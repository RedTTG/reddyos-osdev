#pragma once

#include "vnode.h"

typedef struct mount
{
    const char* name;
    vnode_t* root;
} mount_t;

extern mount_t* root_mount;

// Mount a filesystem vnode at the given path (e.g. "/dev").
// Returns 0 on success, -1 on failure.
int mount_fs(const char* path, vnode_t* new_node);
