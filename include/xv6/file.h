/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-30 00:22:38
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-28 21:59:10
 * @ Description:
 */

#if !defined(__INCLUDE_XV6_FILE_H)
#define __INCLUDE_XV6_FILE_H
#include <xv6/mbr.h>
#include <xv6/sleeplock.h>
#include <stdbool.h>

typedef struct file
{
  enum
  {
    FD_NONE,
    FD_PIPE,
    FD_INODE
  } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
} file_t;

// in-memory copy of an inode
typedef struct inode
{
  uint dev;  // Device number
  uint inum; // Inode number
  int partition;
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
} inode_t;

#define I_VALID 0x2

typedef enum devstate
{
  READY,
  NOTREADY,
} devstate_t;

// table mapping major device number to
// device functions
typedef struct devsw
{
  int (*read)(struct inode *, char *, int, int);
  int (*write)(struct inode *, char *, int, int);
  devstate_t (*getstate)(void);
} devsw_t;

#include "xv6/devnum.h"

extern devsw_t devsw[][MDEV];

//PAGEBREAK!
// Blank page.

#endif // __INCLUDE_XV6_FILE_H
