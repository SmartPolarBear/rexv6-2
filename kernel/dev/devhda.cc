/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-28 21:52:17
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-29 23:51:45
 * @ Description: r/w for hda partitions
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/spinlock.h"
#include "xv6/fs.h"
#include "xv6/file.h"
#include "xv6/buf.h"
#include "xv6/x86.h"

static struct
{
  struct spinlock lock;
  bool locking;
  uint r;
} cons;

//defined in xfs.c
extern int block_off;
extern struct superblock sbs[NPARTITIONS];
extern int current_partition;
extern partition_t partitions[NPARTITIONS];
extern mbr_t mbr;

int hdaread(inode_t *ip, char *dst, int off, int n)
{
  int target;
  target = n;
  off = block_off;
  uint nmeta; //nmeta Number of meta blocks (boot, sb, nlog, inode, bitmap)
  nmeta = 1 + 1 + sbs[ip->partition].nlog + (sbs[ip->partition].ninodes / IPB + 1) + (sbs[ip->partition].size / (BSIZE * 8) + 1);
  if (off > sbs[ip->partition].size)
  {
    cprintf("permission denied\n");
    return 0;
  }

  iunlock(ip);
  if (cons.r == 0)
  {
    cons.r = n;
    struct buf *b;
    uint i;
    while (n > 0)
    {
      b = bread(ROOTDEV, off + partitions[ip->partition].offset);
      for (i = 0; i < BSIZE && n > 0; i++)
      {
        n--;
        dst[i] = b->data[i];
      }
      brelse(b);
    }
    cprintf("read from block %d success\n", off);
  }
  else
    cons.r = 0;
  ilock(ip);
  return target - n;
}
int hdawrite(inode_t *ip, char *cbuf, int off, int n)
{
  // int target;
  // target = n;
  // off = block_off;
  // uint nmeta; //nmeta Number of meta blocks (boot, sb, nlog, inode, bitmap)
  // nmeta = 1 + 1 + sb.nlog + (sb.ninodes / IPB + 1) + (sb.size / (BSIZE * 8) + 1);
  // if (off <= nmeta || off > sb.size)
  // {
  //   cprintf("permission denied %d\n", block_off);
  //   return n;
  // }
  // iunlock(ip);
  // if (cons.r == 0)
  // {
  //   struct buf *b;
  //   uint i;
  //   while (n > 0)
  //   {
  //     b = bget(ROOTDEV, off);
  //     for (i = 0; i < BSIZE && n > 0; i++)
  //     {
  //       n--;
  //       b->data[i] = cbuf[i];
  //     }
  //     bwrite(b);
  //     brelse(b);
  //   }
  //   cprintf("write to block %d success\n", off);
  // }
  // else
  //   cons.r = 0;
  // ilock(ip);
  // return target - n;
  return 0;
}

extern "C" void hdainit(void)
{
  initlock(&cons.lock, "devhda");

  int minors[] = {MDEVHDAP0, MDEVHDAP1, MDEVHDAP2, MDEVHDAP3};

  for (int i = 0; i < NPARTITIONS; i++)
  {
    devsw[NDEVHDA][minors[i]].getstate = [](int major, int minor) {
      return (devstate_t)((mbr.partitions[minor].flags & PART_ALLOCATED) ? READY : NOTREADY);
    };

    devsw[NDEVHDA][minors[i]].write = hdawrite;
    devsw[NDEVHDA][minors[i]].read = hdaread;
  }

  cons.locking = true;
}
