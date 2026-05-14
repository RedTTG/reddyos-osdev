#pragma once
typedef struct vnode vnode_t;

typedef struct vnode_ops
{
    int (*read)(
        vnode_t* node,
        uint64_t offset,
        void* buffer,
        uint64_t size
    );

    int (*write)(
        vnode_t* node,
        uint64_t offset,
        const void* buffer,
        uint64_t size
    );
} vnode_ops_t;

typedef struct vnode
{
    char name[64];

    uint64_t size;

    vnode_ops_t* ops;

    void* internal;
} vnode_t;