#if !defined(__TOOLS_MKFS_BLOCK_H)
#define __TOOLS_MKFS_BLOCK_H
#include "common.h"

namespace block
{
uint freeblock = 0;
void balloc(superblock &sb, int);
} // namespace block

#endif // __TOOLS_MKFS_BLOCK_H
