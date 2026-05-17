#pragma once
#include <stdint.h>
#include "vnode.h"

typedef struct file
{
    vnode_t* vnode;
    uint64_t offset;
    uint32_t refcount;
} file_t;
