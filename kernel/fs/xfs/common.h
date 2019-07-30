#if !defined(__KERNEL_FS_XFS_COMMON_H)
#define __KERNEL_FS_XFS_COMMON_H

#include "xv6/fs.h"
#include "xv6/buf.h"
#include "xv6/file.h"
#include "xv6/mbr.h"

//xfs.c
extern mbr_t mbr;

extern superblock_t sbs[NPARTITIONS];

extern partition_t partitions[NPARTITIONS];

#endif // __KERNEL_FS_XFS_COMMON_H
