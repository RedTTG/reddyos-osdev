#pragma once

#include "vnode.h"

typedef struct mount
{
    vnode_t* root;
} mount_t;

extern mount_t* root_mount;