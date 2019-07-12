/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-23 20:53:03
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-12 23:09:10
 * @ Description:
 */

// File system implementation.  Five layers:
//   + Blocks: allocator for raw disk blocks.
//   + Log: crash recovery for multi-step updates.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.
//
// This file contains the low-level file system manipulation
// routines.  The (higher-level) system call implementations
// are in sysfile.c.

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/mmu.h"
#include "xv6/proc.h"
#include "xv6/spinlock.h"
#include "xv6/sleeplock.h"
#include "xv6/fs.h"
#include "xv6/mount.h"
#include "xv6/buf.h"
#include "xv6/file.h"
#include "xv6/mbr.h"

#define DEFAULT_BOOTPARTITION (-32767)

#define min(a, b) ((a) < (b) ? (a) : (b))
static void itrunc(struct inode *);

mbr_t mbr;

#define MAXNUMINODE 5000

part_t imap[MAXNUMINODE * NPARTITIONS][2];

int entry_lookup(uint inum, int partition);
int insert_mapping(struct inode *ip, int partition_number);

int boot_partition = DEFAULT_BOOTPARTITION;
int current_partition = 0;

// there should be one superblock per disk device
struct superblock sbs[NPARTITIONS] = {};

struct partition partitions[NPARTITIONS] = {};

int checkboot(int index)
{
    return boot_partition == DEFAULT_BOOTPARTITION ? index : boot_partition;
}

//read the master boot record
void readmbr(int dev, mbr_t *mbr)
{
    struct buf *bp;
    bp = bread(dev, 0);
    memmove(mbr, bp->data, sizeof(*mbr));

    char *msgbootable = NULL, *msgtype = NULL;

    //output partition information
    for (int i = 0; i < NPARTITIONS; i++)
    {
        if (mbr->partitions[i].flags & PART_ALLOCATED)
        {
            if (mbr->partitions[i].flags & PART_BOOTABLE)
            {

                msgbootable = "YES";
                boot_partition = checkboot(i);
            }
            else
            {
                msgbootable = "NO";
            }

            if (mbr->partitions[i].type == FS_INODE)
            {
                msgtype = "INODE";
            }
            else
            {
                msgtype = "FAT";
            }

            cprintf("Partition %d: bootable: %s, type:%s, offset:%d, size:%d \n",
                    i, msgbootable, msgtype, mbr->partitions[i].offset,
                    mbr->partitions[i].size);

            //memmove(&partitions[i] + sizeof(uint), &mbr->partitions[i], sizeof(struct dpartition));
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
static void
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
static uint
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
static void
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

// Inodes.
//
// An inode describes a single unnamed file.
// The inode disk structure holds metadata: the file's type,
// its size, the number of links referring to it, and the
// list of blocks holding the file's content.
//
// The inodes are laid out sequentially on disk at
// sb.startinode. Each inode has a number, indicating its
// position on the disk.
//
// The kernel keeps a cache of in-use inodes in memory
// to provide a place for synchronizing access
// to inodes used by multiple processes. The cached
// inodes include book-keeping information that is
// not stored on disk: ip->ref and ip->flags.
//
// An inode and its in-memory represtative go through a
// sequence of states before they can be used by the
// rest of the file system code.
//
// * Allocation: an inode is allocated if its type (on disk)
//   is non-zero. ialloc() allocates, iput() frees if
//   the link count has fallen to zero.
//
// * Referencing in cache: an entry in the inode cache
//   is free if ip->ref is zero. Otherwise ip->ref tracks
//   the number of in-memory pointers to the entry (open
//   files and current directories). iget() to find or
//   create a cache entry and increment its ref, iput()
//   to decrement ref.
//
// * Valid: the information (type, size, &c) in an inode
//   cache entry is only correct when the I_VALID bit
//   is set in ip->flags. ilock() reads the inode from
//   the disk and sets I_VALID, while iput() clears
//   I_VALID if ip->ref has fallen to zero.
//
// * Locked: file system code may only examine and modify
//   the information in an inode and its content if it
//   has first locked the inode.
//
// Thus a typical sequence is:
//   ip = iget(dev, inum)
//   ilock(ip)
//   ... examine and modify ip->xxx ...
//   iunlock(ip)
//   iput(ip)
//
// ilock() is separate from iget() so that system calls can
// get a long-term reference to an inode (as for an open file)
// and only lock it for short periods (e.g., in read()).
// The separation also helps avoid deadlock and races during
// pathname lookup. iget() increments ip->ref so that the inode
// stays cached and pointers to it remain valid.
//
// Many internal file system functions expect the caller to
// have locked the inodes involved; this lets callers create
// multi-step atomic operations.

struct
{
    struct spinlock lock;
    struct inode inode[NINODE];
} icache;

void iinit(int dev)
{
    //init data
    memset(sbs, 0, sizeof(sbs));
    memset(partitions, 0, sizeof(partitions));

    initlock(&icache.lock, "icache");
    readmbr(dev, &mbr);

    for (int i = 0; i < NPARTITIONS; i++)
    {
        initsleeplock(&icache.inode[i].lock, "inode");

        uint nmetai = 1 + 1 + sbs[i].nlog + (sbs[i].ninodes / IPB + 1) + (sbs[i].size / (BSIZE * 8) + 1);

        usablesizes[i] = (sbs[i].size - nmetai) * BSIZE;
        usedsizes[i] = (sbs[i].initusedblock - nmetai) * BSIZE;
        cprintf("fuck:nmeta=%d,initusedblock=%d\n\n", nmetai, sbs[i].initusedblock);
        cprintf("Partition %d: size %d nblocks %d ninodes %d nlog %d logstart %d inodestart %d bmap start %d\n"
                "usedsize %d usablesize %d\n",
                i,
                sbs[i].size,
                sbs[i].nblocks,
                sbs[i].ninodes,
                sbs[i].nlog,
                sbs[i].logstart,
                sbs[i].inodestart,
                sbs[i].bmapstart,
                usedsizes[i], usablesizes[i]);
    }

    // cprintf("Boot partition: size %d nblocks %d ninodes %d nlog %d logstart %d inodestart %d bmap start %d\n", sbs[boot_partition].size,
    //         sbs[boot_partition].nblocks, sbs[boot_partition].ninodes, sbs[current_partition].nlog, sbs[boot_partition].logstart, sbs[boot_partition].inodestart, sbs[boot_partition].bmapstart);

    // uint nmeta; //nmeta Number of meta blocks (boot, sb, nlog, inode, bitmap)
    // nmeta = 1 + 1 + sbs[current_partition].nlog + (sbs[current_partition].ninodes / IPB + 1) + (sbs[current_partition].size / (BSIZE * 8) + 1);

    // useable_capcity = (sbs[current_partition].size - nmeta) * BSIZE;
    // used_capcity = (sbs[current_partition].initusedblock - nmeta) * BSIZE;

    memset(&imap, 0, sizeof(imap));
}

static struct inode *iget(uint dev, uint inum);

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
            ret->part = current_partition;
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
    bp = bread(ip->dev, IBLOCK(ip->inum, sbs[ip->part]) + partitions[ip->part].offset);
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
static struct inode *
iget(uint dev, uint inum)
{
    struct inode *ip, *empty;

    acquire(&icache.lock);

    // Is the inode already cached?
    empty = 0;
    for (ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++)
    {
        if (ip->ref > 0 && ip->dev == dev && ip->inum == inum && ip->part == current_partition)
        {
            ip->ref++;
            release(&icache.lock);
            return ip;
        }
        if (empty == 0 && ip->ref == 0) // Remember empty slot.
            empty = ip;
    }

    // Recycle an inode cache entry.
    if (empty == 0)
        panic("iget: no inodes");

    ip = empty;
    ip->dev = dev;
    ip->inum = inum;
    ip->ref = 1;
    ip->flags = 0;
    ip->part = current_partition;
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
        bp = bread(ip->dev, IBLOCK(ip->inum, sbs[ip->part]) + partitions[ip->part].offset);
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
static uint
bmap(struct inode *ip, uint bn)
{
    uint addr, *a;
    struct buf *bp;

    if (bn < NDIRECT)
    {
        if ((addr = ip->addrs[bn]) == 0)
            ip->addrs[bn] = addr = balloc(ip->dev);
        return addr;
    }
    bn -= NDIRECT;

    if (bn < NINDIRECT)
    {
        // Load indirect block, allocating if necessary.
        if ((addr = ip->addrs[NDIRECT]) == 0)
        {
            //TODO: should be checked if balloc returns a relative bnumber
            ip->addrs[NDIRECT] = addr = balloc(ip->dev);
        }
        bp = bread(ip->dev, addr + partitions[ip->part].offset);
        a = (uint *)bp->data;
        if ((addr = a[bn]) == 0)
        {
            a[bn] = addr = balloc(ip->dev);
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
static void
itrunc(struct inode *ip)
{
    uint i, j;
    struct buf *bp;
    uint *a;

    for (i = 0; i < NDIRECT; i++)
    {
        if (ip->addrs[i])
        {
            bfree(ip->dev, ip->addrs[i]);
            ip->addrs[i] = 0;
        }
    }

    if (ip->addrs[NDIRECT])
    {
        bp = bread(ip->dev, ip->addrs[NDIRECT] + partitions[ip->part].offset);
        a = (uint *)bp->data;
        for (j = 0; j < NINDIRECT; j++)
        {
            if (a[j])
                bfree(ip->dev, a[j]);
        }
        brelse(bp);
        bfree(ip->dev, ip->addrs[NDIRECT]);
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
}

// Get the really device number.
// If ip is a mounted directory, return device number of mounted device.
// Otherwise return original one.
struct inode *
getmntin(struct inode *ip)
{
    if (ip->type != T_DIR)
        return ip;
    struct mountsw *mp;
    for (mp = mountsw; mp < mntswend; mp++)
        if (mp->dp == ip)
            return iget(mp->dev, ROOTINO);
    return ip;
}

//PAGEBREAK!
// Read data from inode.
int readi(struct inode *ip, char *dst, uint off, uint n)
{
    struct mountsw *mp;
    ip = getmntin(ip);
    for (mp = mountsw; mp < mntswend; mp++)
        if (mp->dev == ip->dev)
            return getfs(mp->fsid)->read(ip, dst, off, n);

    return -1;
}

// PAGEBREAK!
// Write data to inode.
int writei(struct inode *ip, char *src, uint off, uint n)
{
    struct mountsw *mp;
    ip = getmntin(ip);
    for (mp = mountsw; mp < mntswend; mp++)
        if (mp->dev == ip->dev)
            return getfs(mp->fsid)->write(ip, src, off, n);
    return -1;
}

int deffsread(struct inode *ip, char *dst, uint off, uint n)
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
        bp = bread(ip->dev, bmap(ip, off / BSIZE) + partitions[ip->part].offset);
        m = min(n - tot, BSIZE - off % BSIZE);
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
int deffswrite(struct inode *ip, char *src, uint off, uint n)
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
        bp = bread(ip->dev, bmap(ip, off / BSIZE) + partitions[ip->part].offset);
        m = min(n - tot, BSIZE - off % BSIZE);
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

//PAGEBREAK!
// Directories

int namecmp(const char *s, const char *t)
{
    return strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
struct inode *
dirlookup(struct inode *dp, char *name, uint *poff)
{
    uint off, inum;
    struct dirent de;
    int partition, old_partition;

    if (dp->type != T_DIR)
        panic("dirlookup not DIR");

    dp = getmntin(dp);

    for (off = 0; off < dp->size; off += sizeof(de))
    {
        if (readi(dp, (char *)&de, off, sizeof(de)) != sizeof(de))
            panic("dirlink read");
        if (de.inum == 0)
            continue;
        if (namecmp(name, de.name) == 0)
        {
            // entry matches path element
            if (poff)
                *poff = off;
            inum = de.inum;
            if ((partition = entry_lookup(inum, dp->part)) != -1)
            {
                old_partition = current_partition;
                current_partition = partition;
                dp = iget(ROOTDEV, ROOTINO);
                current_partition = old_partition;
                return dp;
            }
            return iget(getmntin(dp)->dev, inum);
        }
    }
    return 0;
}

// Write a new directory entry (name, inum) into the directory dp.
int dirlink(struct inode *dp, char *name, uint inum)
{
    uint off;
    struct dirent de;
    struct inode *ip;

    // Check that name is not present.
    if ((ip = dirlookup(dp, name, 0)) != 0)
    {
        iput(ip);
        return -1;
    }

    dp = getmntin(dp);

    // Look for an empty dirent.
    for (off = 0; off < dp->size; off += sizeof(de))
    {
        if (readi(dp, (char *)&de, off, sizeof(de)) != sizeof(de))
            panic("dirlink read");
        if (de.inum == 0)
            break;
    }

    strncpy(de.name, name, DIRSIZ);
    de.inum = inum;
    if (writei(dp, (char *)&de, off, sizeof(de)) != sizeof(de))
        panic("dirlink");

    return 0;
}

//PAGEBREAK!
// Paths

// Copy the next path element from path into name.
// Return a pointer to the element following the copied one.
// The returned path has no leading slashes,
// so the caller can check *path=='\0' to see if the name is the last one.
// If no name to remove, return 0.
//
// Examples:
//   skipelem("a/bb/c", name) = "bb/c", setting name = "a"
//   skipelem("///a//bb", name) = "bb", setting name = "a"
//   skipelem("a", name) = "", setting name = "a"
//   skipelem("", name) = skipelem("////", name) = 0
//
static char *
skipelem(char *path, char *name)
{
    char *s;
    int len;

    while (*path == '/')
        path++;
    if (*path == 0)
        return 0;
    s = path;
    while (*path != '/' && *path != 0)
        path++;
    len = path - s;
    if (len >= DIRSIZ)
        memmove(name, s, DIRSIZ);
    else
    {
        memmove(name, s, len);
        name[len] = 0;
    }
    while (*path == '/')
        path++;
    return path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
static struct inode *
namex(char *path, int nameiparent, char *name)
{
    struct inode *ip, *next;

    if (*path == '/')
        ip = iget(ROOTDEV, ROOTINO);
    else
        ip = idup(proc->cwd);

    while ((path = skipelem(path, name)) != 0)
    {
        ilock(ip);
        if (ip->type != T_DIR)
        {
            iunlockput(ip);
            return 0;
        }
        if (nameiparent && *path == '\0')
        {
            // Stop one level early.
            iunlock(ip);
            return ip;
        }
        if ((next = dirlookup(ip, name, 0)) == 0)
        {
            iunlockput(ip);
            return 0;
        }
        iunlockput(ip);
        ip = next;
    }
    if (nameiparent)
    {
        iput(ip);
        return 0;
    }
    return ip;
}

struct inode *
namei(char *path)
{
    char name[DIRSIZ];
    return namex(path, 0, name);
}

struct inode *
nameiparent(char *path, char *name)
{
    return namex(path, 1, name);
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
    int i;
    for (i = 0; i < NPARTITIONS * MAXNUMINODE; i++)
    {
        if (imap[i][0].inum == ip->inum && imap[i][0].partition == ip->part)
        {
            imap[i][1].inum = ROOTINO;
            imap[i][1].partition = partition_number;
            return 0;
        }
    }
    for (i = 0; i < NPARTITIONS * MAXNUMINODE; i++)
    {
        if (imap[i][0].inum == 0)
        {
            imap[i][0].inum = ip->inum;
            imap[i][0].partition = ip->part;
            imap[i][1].inum = ROOTINO;
            imap[i][1].partition = partition_number;
            return 0;
        }
    }
    cprintf("kernel: insert_mapping failed\n");
    return -1;
}

void switch_partition(int partition)
{
    current_partition = partition;
}
