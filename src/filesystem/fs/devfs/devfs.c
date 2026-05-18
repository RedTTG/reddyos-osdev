#include "common.h"
#include "filesystem/fs/devfs/devfs.h"

static vnode_t dev_nodes[DEVFS_MAX_NODES];
static vnode_t* dev_children[DEVFS_MAX_NODES];

static vnode_t dev_root_node;

void devfs_init(void) {
    memset(dev_nodes, 0, sizeof(dev_nodes));
    memset(dev_children, 0, sizeof(dev_children));
    memset(&dev_root_node, 0, sizeof(dev_root_node));

    strcpy(dev_root_node.name, "dev");
    dev_root_node.internal = dev_children;
    dev_root_node.size = 0; // number of entries
    dev_root_node.ops = NULL; // directory
}

bool devfs_register(const char* name, vnode_ops_t* ops, void* private) {
    // Find free node
    int idx = -1;
    for (int i = 0; i < DEVFS_MAX_NODES; i++) {
        if (dev_nodes[i].name[0] == '\0') {
            idx = i;
            break;
        }
    }
    if (idx < 0)
        return false;

    vnode_t* n = &dev_nodes[idx];
    memset(n, 0, sizeof(vnode_t));
    strncpy(n->name, name, sizeof(n->name)-1);
    n->ops = ops;
    n->internal = private;
    n->size = 0;

    // Append to children
    for (int i = 0; i < DEVFS_MAX_NODES; i++) {
        if (!dev_children[i]) {
            dev_children[i] = n;
            break;
        }
    }

    // Update root size
    int count = 0;
    for (int i = 0; i < DEVFS_MAX_NODES && dev_children[i]; i++) count++;
    dev_root_node.size = count;

    return true;
}

vnode_t* devfs_root(void) {
    return &dev_root_node;
}