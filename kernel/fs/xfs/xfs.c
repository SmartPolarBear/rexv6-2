/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-23 20:53:03
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-30 00:20:26
 * @ Description:
 * File system implementation.  Five layers:
    + Blocks: allocator for raw disk blocks.
    + Log: crash recovery for multi-step updates.
    + Files: inode allocator, reading, writing, metadata.
    + Directories: inode with special contents (list of other inodes!)
    + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.

    This file contains the low-level file system manipulation
    routines.  The (higher-level) system call implementations
    are in sysfile.c.
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

mbr_t mbr;

pair_t imap[MAXNUMINODE * NPARTITIONS][2];

int boot_partition = DEFAULT_BOOTPARTITION;
int current_partition = 0;

// there should be one superblock per disk device
superblock_t sbs[NPARTITIONS] = {};

partition_t partitions[NPARTITIONS] = {};

static inline int checkboot(int index)
{
    return boot_partition == DEFAULT_BOOTPARTITION ? index : boot_partition;
}

//read the master boot record
void readmbr(int dev, mbr_t *mbr)
{
    struct buf *bp;
    bp = bread(dev, 0);
    memmove(mbr, bp->data, sizeof(*mbr));

    //output partition information
    for (int i = 0; i < NPARTITIONS; i++)
    {
        if (mbr->partitions[i].flags & PART_ALLOCATED)
        {
            if (mbr->partitions[i].flags & PART_BOOTABLE)
            {
                boot_partition = checkboot(i);
            }

            current_partition = i;
            partitions[i].dev = dev;
            partitions[i].flags = mbr->partitions[i].flags;
            partitions[i].type = mbr->partitions[i].type;
            partitions[i].offset = mbr->partitions[i].offset;
            partitions[i].size = mbr->partitions[i].size;

            readsb(dev, &sbs[i]);
        }
        current_partition = boot_partition;
    }

    brelse(bp);
}

int entry_lookup(uint inum, int partition)
{
    for (int i = 0; i < NPARTITIONS * MAXNUMINODE; i++)
    {
        if (imap[i][0].inum == inum && imap[i][0].partition == partition)
        {
            return (int)imap[i][1].partition;
        }
    }
    return -1;
}

//inserts a mapping to mapping
//ip is the inode which is being mapped from, partition_number is the partition being mounted to
int insert_mapping(struct inode *ip, int partition_number)
{
    for (int i = 0; i < NPARTITIONS * MAXNUMINODE; i++)
    {
        if (imap[i][0].inum == ip->inum && imap[i][0].partition == ip->partition)
        {
            imap[i][1].inum = ROOTINO;
            imap[i][1].partition = partition_number;
            return 0;
        }
    }

    for (int i = 0; i < NPARTITIONS * MAXNUMINODE; i++)
    {
        if (imap[i][0].inum == 0)
        {
            imap[i][0].inum = ip->inum;
            imap[i][0].partition = ip->partition;
            imap[i][1].inum = ROOTINO;
            imap[i][1].partition = partition_number;
            return 0;
        }
    }
    cprintf("kernel: insert_mapping failed\n");
    return -1;
}

void remove_mapping(inode_t *ip)
{
    for (int i = 0; i < NPARTITIONS * MAXNUMINODE; i++)
    {
        if (imap[i][0].inum == ip->inum && imap[i][0].partition == ip->partition)
        {
            imap[i][1].inum = imap[i][0].inum = 0;
            imap[i][1].partition = imap[i][0].partition = 0;
            break;
            ;
        }
    }
}

void switch_partition(int partition)
{
    current_partition = partition;
}
