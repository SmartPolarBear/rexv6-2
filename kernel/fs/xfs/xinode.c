/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-30 00:07:05
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-30 14:58:20
 * @ Description:
 * Inodes.
    An inode describes a single unnamed file.
    The inode disk structure holds metadata: the file's type,
    its size, the number of links referring to it, and the
    list of blocks holding the file's content.

    The inodes are laid out sequentially on disk at
    sb.startinode. Each inode has a number, indicating its
    position on the disk.

    The kernel keeps a cache of in-use inodes in memory
    to provide a place for synchronizing access
    to inodes used by multiple processes. The cached
    inodes include book-keeping information that is
    not stored on disk: ip->ref and ip->flags.

    An inode and its in-memory represtative go through a
    sequence of states before they can be used by the
    rest of the file system code.

    * Allocation: an inode is allocated if its type (on disk)
    is non-zero. ialloc() allocates, iput() frees if
    the link count has fallen to zero.

    * Referencing in cache: an entry in the inode cache
    is free if ip->ref is zero. Otherwise ip->ref tracks
    the number of in-memory pointers to the entry (open
    files and current directories). iget() to find or
    create a cache entry and increment its ref, iput()
    to decrement ref.

    * Valid: the information (type, size, &c) in an inode
    cache entry is only correct when the I_VALID bit
    is set in ip->flags. ilock() reads the inode from
    the disk and sets I_VALID, while iput() clears
    I_VALID if ip->ref has fallen to zero.

    * Locked: file system code may only examine and modify
    the information in an inode and its content if it
    has first locked the inode.

    Thus a typical sequence is:
    ip = iget(dev, inum)
    ilock(ip)
    ... examine and modify ip->xxx ...
    iunlock(ip)
    iput(ip)

    ilock() is separate from iget() so that system calls can
    get a long-term reference to an inode (as for an open file)
    and only lock it for short periods (e.g., in read()).
    The separation also helps avoid deadlock and races during
    pathname lookup. iget() increments ip->ref so that the inode
    stays cached and pointers to it remain valid.

    Many internal file system functions expect the caller to
    have locked the inodes involved; this lets callers create
    multi-step atomic operations.
 */

#include "common.h"
#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/spinlock.h"

#include "xblock.h"

extern int current_partition;

extern pair_t imap[MAXNUMINODE * NPARTITIONS][2];

struct
{
    spinlock_t lock;
    inode_t inode[NINODE];
} icache;

void iinit(int dev)
{
    //init data
    memset(sbs, 0, sizeof(sbs));
    memset(partitions, 0, sizeof(partitions));
    memset(&imap, 0, sizeof(imap));

    initlock(&icache.lock, "icache");
    readmbr(dev, &mbr);

    for (int i = 0; i < NPARTITIONS; i++)
    {
        initsleeplock(&icache.inode[i].lock, "inode");

        uint nmetai = 1 + 1 + sbs[i].nlog + (sbs[i].ninodes / IPB + 1) + (sbs[i].size / (BSIZE * 8) + 1);

        usablesizes[i] = (sbs[i].size - nmetai) * BSIZE;
        usedsizes[i] = (sbs[i].initusedblock - nmetai) * BSIZE;
        // cprintf("Partition %d: size %d nblocks %d ninodes %d nlog %d logstart %d inodestart %d bmap start %d\n"
        //         "usedsize %d usablesize %d\n",
        //         i,
        //         sbs[i].size,
        //         sbs[i].nblocks,
        //         sbs[i].ninodes,
        //         sbs[i].nlog,
        //         sbs[i].logstart,
        //         sbs[i].inodestart,
        //         sbs[i].bmapstart,
        //         usedsizes[i], usablesizes[i]);
    }

    // cprintf("Boot partition: size %d nblocks %d ninodes %d nlog %d logstart %d inodestart %d bmap start %d\n", sbs[boot_partition].size,
    //         sbs[boot_partition].nblocks, sbs[boot_partition].ninodes, sbs[current_partition].nlog, sbs[boot_partition].logstart, sbs[boot_partition].inodestart, sbs[boot_partition].bmapstart);

    // uint nmeta; //nmeta Number of meta blocks (boot, sb, nlog, inode, bitmap)
    // nmeta = 1 + 1 + sbs[current_partition].nlog + (sbs[current_partition].ninodes / IPB + 1) + (sbs[current_partition].size / (BSIZE * 8) + 1);

    // useable_capcity = (sbs[current_partition].size - nmeta) * BSIZE;
    // used_capcity = (sbs[current_partition].initusedblock - nmeta) * BSIZE;
}

struct inode *iget(uint dev, uint inum);

//PAGEBREAK!
// Allocate a new inode with the given type on device dev.
// A free inode has a type of zero.
struct inode *
ialloc(uint dev, short type)
{
    uint inum;
    struct buf *bp;
    struct dinode *dip;

    for (inum = 1; inum < sbs[current_partition].ninodes; inum++)
    {
        bp = bread(dev, IBLOCK(inum, sbs[current_partition]) + partitions[current_partition].offset);

        dip = (struct dinode *)bp->data + inum % IPB;
        if (dip->type == 0)
        { // a free inode
            memset(dip, 0, sizeof(*dip));
            dip->type = type;
            log_write(bp); // mark it allocated on the disk
            brelse(bp);
            struct inode *ret = iget(dev, inum);
            ret->partition = current_partition;
            return ret;
        }
        brelse(bp);
    }
    panic("ialloc: no inodes");
}

// Copy a modified in-memory inode to disk.
void iupdate(struct inode *ip)
{
    struct buf *bp;
    struct dinode *dip;
    bp = bread(ip->dev, IBLOCK(ip->inum, sbs[ip->partition]) + partitions[ip->partition].offset);
    dip = (struct dinode *)bp->data + ip->inum % IPB;
    dip->type = ip->type;
    dip->major = ip->major;
    dip->minor = ip->minor;
    dip->nlink = ip->nlink;
    dip->size = ip->size;
    memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
    log_write(bp);
    brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
struct inode *iget(uint dev, uint inum)
{
    inode_t *empty = NULL;

    acquire(&icache.lock);

    // Is the inode already cached?
    for (inode_t *ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++)
    {
        if (ip->ref > 0 && ip->dev == dev && ip->inum == inum && ip->partition == current_partition)
        {
            ip->ref++;
            release(&icache.lock);
            return ip; //cached, return
        }

        if (empty == 0 && ip->ref == 0)
        {
            empty = ip; // Remember empty slot.
        }
    }

    // Recycle an inode cache entry.
    if (empty == 0)
    {
        panic("iget: no empty inodes");
    }

    inode_t *ip = empty;
    ip->dev = dev;
    ip->inum = inum;
    ip->ref = 1;
    ip->flags = 0;
    ip->partition = current_partition;
    ip->partitions = partitions;

    release(&icache.lock);
    return ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
struct inode *
idup(struct inode *ip)
{
    acquire(&icache.lock);
    ip->ref++;
    release(&icache.lock);
    return ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
void ilock(struct inode *ip)
{
    struct buf *bp;
    struct dinode *dip;

    if (ip == 0 || ip->ref < 1)
        panic("ilock");

    acquiresleep(&ip->lock);

    if (!(ip->flags & I_VALID))
    {
        bp = bread(ip->dev, IBLOCK(ip->inum, sbs[ip->partition]) + partitions[ip->partition].offset);
        dip = (struct dinode *)bp->data + ip->inum % IPB;
        ip->type = dip->type;
        ip->major = dip->major;
        ip->minor = dip->minor;
        ip->nlink = dip->nlink;
        ip->size = dip->size;
        ip->partitions = partitions;
        memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
        brelse(bp);
        ip->flags |= I_VALID;
        if (ip->type == 0)
        {
            cprintf("inum:%d\n", ip->inum);
            panic("ilock: no type");
        }
    }
}

// Unlock the given inode.
void iunlock(struct inode *ip)
{
    if (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
        panic("iunlock");

    releasesleep(&ip->lock);
}

// Drop a reference to an in-memory inode.
// If that was the last reference, the inode cache entry can
// be recycled.
// If that was the last reference and the inode has no links
// to it, free the inode (and its content) on disk.
// All calls to iput() must be inside a transaction in
// case it has to free the inode.
void iput(struct inode *ip)
{
    acquire(&icache.lock);
    if (ip->ref == 1 && (ip->flags & I_VALID) && ip->nlink == 0)
    {
        // inode has no links and no other references: truncate and free.
        release(&icache.lock);
        itrunc(ip);
        ip->type = 0;
        iupdate(ip);
        acquire(&icache.lock);
        ip->flags = 0;
    }
    ip->ref--;
    release(&icache.lock);
}

// Common idiom: unlock, then put.
void iunlockput(struct inode *ip)
{
    iunlock(ip);
    iput(ip);
}

//PAGEBREAK!
// Inode content
//
// The content (data) associated with each inode is stored
// in blocks on the disk. The first NDIRECT block numbers
// are listed in ip->addrs[].  The next NINDIRECT blocks are
// listed in block ip->addrs[NDIRECT].

// Return the disk block address of the nth block in inode ip.
// If there is no such block, bmap allocates one.
uint bmap(struct inode *ip, uint bn)
{
    uint addr, *a;
    struct buf *bp;

    if (bn < NDIRECT)
    {
        if ((addr = ip->addrs[bn]) == 0)
            ip->addrs[bn] = addr = balloc(ip->dev, current_partition);
        return addr;
    }
    bn -= NDIRECT;

    if (bn < NINDIRECT)
    {
        // Load indirect block, allocating if necessary.
        if ((addr = ip->addrs[NDIRECT]) == 0)
        {
            //TODO: should be checked if balloc returns a relative bnumber
            ip->addrs[NDIRECT] = addr = balloc(ip->dev, current_partition);
        }
        bp = bread(ip->dev, addr + partitions[ip->partition].offset);
        a = (uint *)bp->data;
        if ((addr = a[bn]) == 0)
        {
            a[bn] = addr = balloc(ip->dev, current_partition);
            log_write(bp);
        }
        brelse(bp);
        return addr;
    }

    panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Only called when the inode has no links
// to it (no directory entries referring to it)
// and has no in-memory reference to it (is
// not an open file or current directory).
void itrunc(struct inode *ip)
{
    uint i, j;
    struct buf *bp;
    uint *a;

    for (i = 0; i < NDIRECT; i++)
    {
        if (ip->addrs[i])
        {
            bfree(ip->dev, ip->addrs[i], current_partition);
            ip->addrs[i] = 0;
        }
    }

    if (ip->addrs[NDIRECT])
    {
        bp = bread(ip->dev, ip->addrs[NDIRECT] + partitions[ip->partition].offset);
        a = (uint *)bp->data;
        for (j = 0; j < NINDIRECT; j++)
        {
            if (a[j])
                bfree(ip->dev, a[j], current_partition);
        }
        brelse(bp);
        bfree(ip->dev, ip->addrs[NDIRECT], current_partition);
        ip->addrs[NDIRECT] = 0;
    }

    ip->size = 0;
    iupdate(ip);
}

// Copy stat information from inode.
void stati(struct inode *ip, stat_t *st)
{
    st->dev = ip->dev;
    st->ino = ip->inum;
    st->type = ip->type;
    st->nlink = ip->nlink;
    st->size = ip->size;
    st->major = ip->major;
    st->minor = ip->minor;
}

//PAGEBREAK!
// Read data from inode.
int readi(struct inode *ip, char *dst, uint off, uint n)
{
    uint tot, m;
    struct buf *bp;

    if (ip->type == T_DEV)
    {
        if (ip->major < 0 || ip->minor < 0 || ip->major >= NDEV || ip->minor >= MDEV || !devsw[ip->major][ip->minor].read)
            return -1;

        return devsw[ip->major][ip->minor].read(ip, dst, off, n);
    }

    if (off > ip->size || off + n < off)
        return -1;
    if (off + n > ip->size)
        n = ip->size - off;

    for (tot = 0; tot < n; tot += m, off += m, dst += m)
    {
        bp = bread(ip->dev, bmap(ip, off / BSIZE) + partitions[ip->partition].offset);
        m = MIN(n - tot, BSIZE - off % BSIZE);
        /*
        cprintf("data off %d:\n", off);
        for (int j = 0; j < min(m, 10); j++) {
          cprintf("%x ", bp->data[off%BSIZE+j]);
        }
        cprintf("\n");
        */
        memmove(dst, bp->data + off % BSIZE, m);
        brelse(bp);
    }

    return n;
}

// PAGEBREAK!
// Write data to inode.
int writei(struct inode *ip, char *src, uint off, uint n)
{
    uint tot, m;
    struct buf *bp;

    if (ip->type == T_DEV)
    {
        if (ip->major < 0 || ip->minor < 0 || ip->major >= NDEV || ip->minor >= MDEV || !devsw[ip->major][ip->minor].write)
            return -1;
        return devsw[ip->major][ip->minor].write(ip, src, off, n);
    }

    if (off > ip->size || off + n < off)
        return -1;
    if (off + n > MAXFILE * BSIZE)
        return -1;

    for (tot = 0; tot < n; tot += m, off += m, src += m)
    {
        bp = bread(ip->dev, bmap(ip, off / BSIZE) + partitions[ip->partition].offset);
        m = MIN(n - tot, BSIZE - off % BSIZE);
        memmove(bp->data + off % BSIZE, src, m);
        log_write(bp);
        brelse(bp);
    }

    if (n > 0 && off > ip->size)
    {
        ip->size = off;
        iupdate(ip);
    }
    return n;
}