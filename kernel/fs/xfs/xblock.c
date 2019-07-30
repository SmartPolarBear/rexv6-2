/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-30 00:06:11
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-30 14:04:02
 * @ Description:
 */

#include "common.h"
#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/spinlock.h"

// Read the super block.
void readsb(int dev, struct superblock *sb, int partition)
{
    struct buf *bp;

    bp = bread(dev, mbr.partitions[partition].offset);
    memmove(sb, bp->data, sizeof(*sb));

    sb->offset = mbr.partitions[partition].offset;

    brelse(bp);
}

// Zero a block.
void bzero(int dev, int bno, int partition)
{
    struct buf *bp;

    bp = bread(dev, bno + partitions[partition].offset);
    memset(bp->data, 0, BSIZE);
    log_write(bp);

    brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
uint balloc(uint dev, int partition)
{
    uint b, bi, m;
    struct buf *bp;

    bp = 0;
    for (b = 0; b < sbs[partition].size; b += BPB)
    {
        bp = bread(dev, BBLOCK(b, sbs[partition]) + partitions[partition].offset);
        for (bi = 0; bi < BPB && b + bi < sbs[partition].size; bi++)
        {
            m = 1 << (bi % 8);
            if ((bp->data[bi / 8] & m) == 0)
            {                          // Is block free?
                bp->data[bi / 8] |= m; // Mark block in use.
                log_write(bp);
                brelse(bp);
                bzero(dev, b + bi, partition);
                // used_capcity += BSIZE;
                usedsizes[partition] += BSIZE;
                //cprintf("2 used capcity : %d KB\n", used_capcity/KBSIZE);
                return b + bi;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
}

// Free a disk block.
void bfree(int dev, uint b, int partition)
{
    struct buf *bp;
    int bi, m;

    readsb(dev, &sbs[partition], partition);
    bp = bread(dev, BBLOCK(b, sbs[partition]) + partitions[partition].offset);
    bi = b % BPB;
    m = 1 << (bi % 8);
    if ((bp->data[bi / 8] & m) == 0)
        panic("freeing free block");
    bp->data[bi / 8] &= ~m;
    log_write(bp);
    brelse(bp);
    // used_capcity -= BSIZE;
    usedsizes[partition] -= BSIZE;
}