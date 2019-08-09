#if !defined(__TOOLS_MKFS_INODE_H)
#define __TOOLS_MKFS_INODE_H
#include "common.h"

namespace inode
{
uint freeinode = 1;

void winode(struct superblock &sb, uint, struct dinode *);
void rinode(struct superblock &sb, uint inum, struct dinode *ip);
uint ialloc(struct superblock &sb, ushort type);
void iappend(struct superblock &sb, uint inum, void *p, int n);
} // namespace inode

#endif // __TOOLS_MKFS_INODE_H
