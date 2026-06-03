#pragma once
#include "stat.h"
typedef struct vnode vnode_t;
typedef struct vm_area vm_area_t;

typedef struct vnode_ops
{
    int (*read)(
        vnode_t* node,
        u64 offset,
        void* buffer,
        u64 size
    );

    int (*write)(
        vnode_t* node,
        u64 offset,
        const void* buffer,
        u64 size
    );

    int (*ioctl)(
        vnode_t* node,
        u64 cmd,
        u64 arg
        );
    bool (*vma_fault)(
        vnode_t* node,
        vm_area_t* vma,
        u64 page
    );
} vnode_ops_t;

typedef struct vnode
{
    char name[64];

    int64_t size;

    vnode_ops_t* ops;

    void* internal;
} vnode_t;