/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-26 23:39:12
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-30 00:04:49
 * @ Description:
 */


#if !defined(__INCLUDE_XV6_FS_H)
#define __INCLUDE_XV6_FS_H

#include "xv6/mbr.h"
#if defined(__cplusplus)
extern "C"
{
#endif
// On-disk file system format.
// Both the kernel and user programs use this header file.

#define ROOTINO 1 // root i-number
#define BSIZE 512 // block size
#define KBSIZE 1000
#define OFFSET (FSSIZE * current_partition)

#define DEFAULT_BOOTPARTITION (-32767)
#define MAXNUMINODE 5000

// extern int used_capcity, useable_capcity;

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
typedef struct superblock
{
  uint size;          // Size of file system image (blocks)
  uint nblocks;       // Number of data blocks
  uint ninodes;       // Number of inodes.
  uint nlog;          // Number of log blocks
  uint logstart;      // Block number of first log block
  uint inodestart;    // Block number of first inode block
  uint bmapstart;     // Block number of first free map block
  uint initusedblock; // Block number of used  block
  uint offset;
} superblock_t;

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode
{
  short type;              // File type
  short major;             // Major device number (T_DEV only)
  short minor;             // Minor device number (T_DEV only)
  short nlink;             // Number of links to inode in file system
  uint size;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

enum support_fs
{
  XV6FS,
};

// Inodes per block.
#define IPB (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB (BSIZE * 8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) (b / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent
{
  ushort inum;
  char name[DIRSIZ];
};

typedef struct pair
{
  char inum;
  char partition;
} pair_t;

extern int usablesizes[NPARTITIONS];
extern int usedsizes[NPARTITIONS];

#if defined(__cplusplus)
}
#endif

#endif // __INCLUDE_XV6_FS_H
