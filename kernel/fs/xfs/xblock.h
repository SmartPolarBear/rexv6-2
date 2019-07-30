#if !defined(__KERNEL_FS_XFS_XBLOCK_H)
#define __KERNEL_FS_XFS_XBLOCK_H

void readsb(int dev, struct superblock *sb, int partition);
void bzero(int dev, int bno, int partition);
uint balloc(uint dev, int partition);
void bfree(int dev, uint b, int partition);

#endif // __KERNEL_FS_XFS_XBLOCK_H
