/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-26 23:01:12
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-26 23:38:48
 * @ Description:
 */

//original file is provided by OS162 course.
//https://www.cs.bgu.ac.il/~os162/wiki.files/mbr.h
#if !defined(__INCLUDE_XV6_MBR_H)
#define __INCLUDE_XV6_MBR_H

#include "xv6/types.h"

#define BOOTSTRAP 		(446)

#define NPARTITIONS 	(4)

#define PART_ALLOCATED 	(1) // allocated partition
#define PART_BOOTABLE 	(2) // bootable partition

#define FS_INODE 		(0) // inode based partition
#define FS_FAT 			(1) // fat based partition

typedef struct dpartition
{
	uint flags;
	uint type;
	uint offset;
	uint size;
} dpartition_t;

#pragma pack(1) // prevents the compiler from aligning (padding) generated code for 4 byte boundary
struct mbr
{
	uchar bootstrap[BOOTSTRAP];
	struct dpartition partitions[NPARTITIONS];
	uchar magic[2];
};

typedef struct mbr mbr_t;

typedef struct partition
{
	uint dev;

	uint flags;
	uint type;
	uint offset;
	uint size;
} partition_t;

#endif // __INCLUDE_XV6_MBR_H
