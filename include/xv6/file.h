/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-30 00:22:38
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-03 23:12:46
 * @ Description:
 */

#if !defined(__INCLUDE_XV6_FILE_H)
#define __INCLUDE_XV6_FILE_H
#include <xv6/mbr.h>

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
};


// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int part;
  int ref;            // Reference count
  struct sleeplock lock;
  int flags;          // I_VALID

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1];
  partition_t *partitions;
};
#define I_VALID 0x2

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, int, int);
  int (*write)(struct inode*, char*, int, int);
};

extern struct devsw devsw[][MDEV];

#define NDEVHDA  2
#define MDEVHDA  1
#define NDEVHDAINFO 2
#define MDEVHDAINFO 2
#define NCONSOLE 1
#define MCONSOLE 1
#define NDEVSOUND 1
#define MDEVSOUND 2
#define NDEVNULL 1
#define MDEVNULL 3
#define NDEVZERO 1
#define MDEVZERO 4
#define NDEVRANDOM 1
#define MDEVRANDOM 5
#define NDEVURANDOM 1
#define MDEVURANDOM 6
#define NDEVPERFCTR 1
#define MDEVPERFCTR 7
#define NDEVFULL 1
#define MDEVFULL 8

//PAGEBREAK!
// Blank page.


#endif // __INCLUDE_XV6_FILE_H
