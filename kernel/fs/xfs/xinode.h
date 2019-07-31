#if !defined(__INCLUDE_KERNEL_FS_XFS_XINODE_H)
#define __INCLUDE_KERNEL_FS_XFS_XINODE_H

#include "xv6/types.h"
#include "xv6/stat.h"

void iinit(int dev);
inode_t *iget(uint dev, uint inum,int partition);
struct inode *ialloc(uint dev, short type);
void iupdate(struct inode *ip);
struct inode *idup(struct inode *ip);
void ilock(struct inode *ip);
void iunlock(struct inode *ip);
void iput(struct inode *ip);
void iunlockput(struct inode *ip);
uint bmap(struct inode *ip, uint bn);
void itrunc(struct inode *ip);
void stati(struct inode *ip, stat_t *st);
int readi(struct inode *ip, char *dst, uint off, uint n);
int writei(struct inode *ip, char *src, uint off, uint n);
#endif // __INCLUDE_KERNEL_FS_XFS_XINODE_H
