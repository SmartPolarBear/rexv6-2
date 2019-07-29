/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-30 00:06:11
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-30 00:11:08
 * @ Description:
 */


#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/spinlock.h"
#include "xv6/fs.h"
#include "xv6/buf.h"
#include "xv6/file.h"
#include "xv6/mbr.h"


extern mbr_t mbr;

extern int current_partition;

extern superblock_t sbs[NPARTITIONS];

extern partition_t partitions[NPARTITIONS];


// Read the super block.
void readsb(int dev, struct superblock *sb)
{
    struct buf *bp;

    bp = bread(dev, mbr.partitions[current_partition].offset);
    memmove(sb, bp->data, sizeof(*sb));

    sb->offset = mbr.partitions[current_partition].offset;

    brelse(bp);
}

// Zero a block.
void
bzero(int dev, int bno)
{
    struct buf *bp;

    bp = bread(dev, bno + partitions[current_partition].offset);
    memset(bp->data, 0, BSIZE);
    log_write(bp);

    brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
uint
balloc(uint dev)
{
    uint b, bi, m;
    struct buf *bp;

    bp = 0;
    for (b = 0; b < sbs[current_partition].size; b += BPB)
    {
        bp = bread(dev, BBLOCK(b, sbs[current_partition]) + partitions[current_partition].offset);
        for (bi = 0; bi < BPB && b + bi < sbs[current_partition].size; bi++)
        {
            m = 1 << (bi % 8);
            if ((bp->data[bi / 8] & m) == 0)
            {                          // Is block free?
                bp->data[bi / 8] |= m; // Mark block in use.
                log_write(bp);
                brelse(bp);
                bzero(dev, b + bi);
                // used_capcity += BSIZE;
                usedsizes[current_partition] += BSIZE;
                //cprintf("2 used capcity : %d KB\n", used_capcity/KBSIZE);
                return b + bi;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
}

// Free a disk block.
void
bfree(int dev, uint b)
{
    struct buf *bp;
    int bi, m;

    readsb(dev, &sbs[current_partition]);
    bp = bread(dev, BBLOCK(b, sbs[current_partition]) + partitions[current_partition].offset);
    bi = b % BPB;
    m = 1 << (bi % 8);
    if ((bp->data[bi / 8] & m) == 0)
        panic("freeing free block");
    bp->data[bi / 8] &= ~m;
    log_write(bp);
    brelse(bp);
    // used_capcity -= BSIZE;
    usedsizes[current_partition] -= BSIZE;
}