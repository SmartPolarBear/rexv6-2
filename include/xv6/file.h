/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-30 00:22:38
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-10 23:36:57
 * @ Description:
 */

#if !defined(__INCLUDE_XV6_FILE_H)
#define __INCLUDE_XV6_FILE_H
#include <xv6/mbr.h>
#include <xv6/sleeplock.h>


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
typedef struct inode {
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
}inode_t;
#define I_VALID 0x2

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, int, int);
  int (*write)(struct inode*, char*, int, int);
};

#include "xv6/devnum.h"

extern struct devsw devsw[][MDEV];


//PAGEBREAK!
// Blank page.


#endif // __INCLUDE_XV6_FILE_H
