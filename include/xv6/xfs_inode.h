#if !defined(__INCLUDE_XV6_XFS_INODE_H)
#define __INCLUDE_XV6_XFS_INODE_H
#include "xv6/types.h"
#include "xv6/mbr.h"
#include "xv6/sleeplock.h"

// On-disk file system format.
// Both the kernel and user programs use this header file.

#define ROOTINO 1 // root i-number
#define BSIZE 512 // block size
#define KBSIZE 1000
#define OFFSET (FSSIZE * current_partition)

extern int used_capcity, useable_capcity;
// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct xfs_superblock
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
};

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct xfs_dinode
{
  short type;              // File type
  short major;             // Major device number (T_DEV only)
  short minor;             // Minor device number (T_DEV only)
  short nlink;             // Number of links to inode in file system
  uint size;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// in-memory copy of an inode
typedef struct xfs_inode
{
  uint dev;  // Device number
  uint inum; // Inode number
  int part;
  int ref; // Reference count
  struct sleeplock lock;
  int flags; // I_VALID

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
  partition_t *partitions;
} xfs_inode_t;

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

struct xfs_dirent
{
  ushort inum;
  char name[DIRSIZ];
};

typedef struct xfs_pair
{
  char inum;
  char partition;
} part_t;


#endif // __INCLUDE_XV6_XFS_INODE_H
