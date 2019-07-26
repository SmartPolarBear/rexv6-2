#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/inode.h"

/* *
 * inode_init - initialize a inode structure
 * invoked by vop_init
 * */
void inode_init(inode_t *node, const inode_ops_t *ops, int fstype)
{
    node->ops = ops;
    node->type = fstype;
}