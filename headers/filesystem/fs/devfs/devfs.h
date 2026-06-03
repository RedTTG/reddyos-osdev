#pragma once

#define DEVFS_MAX_NODES 64

void devfs_init(void);

vnode_t* devfs_register(
    const char* name,
    vnode_ops_t* ops,
    void* private
);

vnode_t* devfs_root(void);