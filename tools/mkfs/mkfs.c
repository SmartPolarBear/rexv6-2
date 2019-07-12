/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-06 00:10:55
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-12 23:02:40
 * @ Description:
 */


#define NONKERNEL

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <assert.h>

#define dirent xv6dirent
#define stat xv6stat
#include "xv6/types.h"
#include "xv6/fs.h"
#include "xv6/stat.h"
#include "xv6/param.h"
#include "xv6/mbr.h"

#undef dirent
#undef stat

#ifndef xv6static_assert
#define xv6static_assert(a, b) \
    do                         \
    {                          \
        switch (0)             \
        case 0:                \
        case (a):;             \
    } while (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

int nbitmap = FSSIZE / (BSIZE * 8) + 1;
int ninodeblocks = NINODES / IPB + 1;
int nlog = LOGSIZE;
int nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nblocks; // Number of data blocks

int fsfd;
struct superblock sbs[4];
mbr_t mbr;
char zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;
uint master_freeblock;
int current_partition = 0;
dpartition_t partitions[4];

void balloc(int);
void wsect(uint, void *, int);
void winode(uint, struct dinode *);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf, int);
uint ialloc(ushort type, int mtime);
void iappend(uint inum, void *p, int n);
void dappend(int dirino, char *name, int fileino);
void add_directory(int dirino, char *localdir);
int makdir(int dirino, char *newdir, struct stat *sb);

// convert to intel byte order
ushort
xshort(ushort x)
{
    ushort y;
    uchar *a = (uchar *)&y;
    a[0] = x;
    a[1] = x >> 8;
    return y;
}

uint xint(uint x)
{
    uint y;
    uchar *a = (uchar *)&y;
    a[0] = x;
    a[1] = x >> 8;
    a[2] = x >> 16;
    a[3] = x >> 24;
    return y;
}

int main(int argc, char *argv[])
{
    int i, fd_bootblock, fd_kernel, blocks_for_kernel;
    uint rootino, off;
    char buf[BSIZE];
    struct dinode din;

    xv6static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

    if (argc != 5)
    {
        fprintf(stderr, "Usage: mkfs fs.img basedir bootblock\n");
        exit(1);
    }

    assert((BSIZE % sizeof(struct dinode)) == 0);
    assert((BSIZE % sizeof(struct xv6dirent)) == 0);

    // Open the filesystem image file
    fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fsfd < 0)
    {
        perror(argv[1]);
        exit(1);
    }

    // 1 fs block = 1 disk sector
    // Number of meta blocks: boot block, superblock, log blocks,
    // i-node blocks and the free bitmap blocks
    nmeta = 1 + nlog + ninodeblocks + nbitmap;
    // Now work out how many free blocks are left
    nblocks = FSSIZE - nmeta;

    //Setup mbr
    memset(&mbr, 0, sizeof(mbr));

    for (i = 0; i < 2; i++)
        wsect(i, zeroes, 1);

    //write kernel to block 1
    fd_kernel = open(argv[4], O_RDONLY, 0666);
    printf("kernel:%s\n", argv[4]);
    memset(buf, 0, sizeof(buf));
    blocks_for_kernel = 0;
    while ((read(fd_kernel, buf, sizeof(buf))) > 0)
    {
        blocks_for_kernel++;
        wsect(blocks_for_kernel, buf, 1); // writes to absolute block #block_for_kernel
    }
    close(fd_kernel);

    //copy bootblock into mbr bootstart
    fd_bootblock = open(argv[3], O_RDONLY, 0666);
    printf("bootblock:%s\n", argv[3]);
    read(fd_bootblock, &mbr.bootstrap[0], sizeof(mbr.bootstrap));

    //set boot signature
    lseek(fd_bootblock, sizeof(mbr) - sizeof(mbr.magic), SEEK_SET);
    read(fd_bootblock, mbr.magic, sizeof(mbr.magic));

    close(fd_bootblock);

    //allocate partitions
    memset(&partitions, 0, sizeof(struct dpartition) * 4);

    for (i = 0; i < NPARTITIONS; i++)
    {
        mbr.partitions[i].flags = i == 0 ? PART_ALLOCATED | PART_BOOTABLE : PART_ALLOCATED;
        mbr.partitions[i].type = FS_INODE;
        mbr.partitions[i].offset = i == 0 ? blocks_for_kernel + 1 : mbr.partitions[i - 1].offset + mbr.partitions[i - 1].size;
        mbr.partitions[i].size = FSSIZE;
        partitions[i].offset = mbr.partitions[i].offset;
    }

    // initialize super blocks
    for (i = 0; i < NPARTITIONS; i++)
    {
        sbs[i].size = xint(FSSIZE);
        sbs[i].nblocks = xint(nblocks);
        sbs[i].ninodes = xint(NINODES);
        sbs[i].nlog = xint(nlog);
        sbs[i].logstart = xint(1);
        sbs[i].inodestart = xint(1 + nlog);
        sbs[i].bmapstart = xint(1 + nlog + ninodeblocks);
        sbs[i].offset = partitions[i].offset;
    }

    printf("Each partition has the following composition: nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n", nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

    freeblock = nmeta; // The first free block that we can allocate
    master_freeblock = freeblock;

    //write mbr
    memset(buf, 0, sizeof(buf));
    memmove(buf, &mbr, sizeof(mbr));
    wsect(0, buf, 1);

    // allocate partitions
    for (i = 0; i < FSSIZE * NPARTITIONS; i++)
        wsect(i + 1 + blocks_for_kernel, zeroes, 1);

    // // Mark the in-use blocks in the free block list;
    for (i = 0; i < NPARTITIONS; i++)
    {
        memset(buf, 0, sizeof(buf));
        memmove(buf, &sbs[i], sizeof(sbs[i]));
        current_partition = i;
        wsect(0, buf, 0);
    }
    current_partition = 0;
    
    sbs[current_partition].initusedblock = freeblock;

    // allocate inode for root directory in each partition
    for (i = 0; i < NPARTITIONS; i++, current_partition++, freeinode = 1)
    {
        // Grab an i-node for the root directory
        rootino = ialloc(T_DIR, 0); // Epoch mtime for now
        assert(rootino == ROOTINO);

        // Set up the directory entry for . and add it to the root dir
        // Set up the directory entry for .. and add it to the root dir
        dappend(rootino, ".", rootino);
        dappend(rootino, "..", rootino);
    }
    current_partition = 0;
    freeinode = 4;

    // populate the blocks bitmap for partitions 1-3
    for (i = 1; i < NPARTITIONS; i++)
    {
        current_partition = i;
        // fix size of root inode dir
        rinode(rootino, &din);
        off = xint(din.size);
        off = ((off / BSIZE) + 1) * BSIZE;
        din.size = xint(off);
        winode(rootino, &din);

        balloc(freeblock);
    }
    current_partition = 0;

    // Add the contents of the command-line directory to the root dir
    add_directory(rootino, argv[2]);

    // Fix the size of the root inode dir
    rinode(rootino, &din);
    off = xint(din.size);
    off = ((off / BSIZE) + 1) * BSIZE;
    din.size = xint(off);

    winode(rootino, &din);

    balloc(freeblock);

    exit(0);
}

// Write a sector to the image
void wsect(uint sec, void *buf, int mbr)
{
    uint off = mbr ? 0 : partitions[current_partition].offset;
    if (lseek(fsfd, (off + sec) * BSIZE, 0) != (off + sec) * BSIZE)
    {
        perror("lseek");
        exit(1);
    }
    if (write(fsfd, buf, BSIZE) != BSIZE)
    {
        perror("write");
        exit(1);
    }
}

// Write an i-node to the image
void winode(uint inum, struct dinode *ip)
{
    char buf[BSIZE];
    uint bn;
    struct dinode *dip;

    bn = IBLOCK(inum, sbs[current_partition]);
    rsect(bn, buf, 0);
    dip = ((struct dinode *)buf) + (inum % IPB);
    *dip = *ip;
    wsect(bn, buf, 0);
}

// Read an i-node from the image
void rinode(uint inum, struct dinode *ip)
{
    char buf[BSIZE];
    uint bn;
    struct dinode *dip;

    bn = IBLOCK(inum, sbs[current_partition]);
    rsect(bn, buf, 0);
    dip = ((struct dinode *)buf) + (inum % IPB);
    *ip = *dip;
}

// Read a sector from the image
void rsect(uint sec, void *buf, int mbr)
{
    int i = -111;
    uint off = mbr ? 0 : partitions[current_partition].offset;
    if (lseek(fsfd, (off + sec) * BSIZE, 0) != (off + sec) * BSIZE)
    {
        perror("lseek");
        exit(1);
    }
    if ((i = read(fsfd, buf, BSIZE)) != BSIZE)
    {
        perror("read");
        exit(1);
    }
}

// Allocate an i-node
uint ialloc(ushort type, int mtime)
{
    uint inum = freeinode++;
    struct dinode din;

    assert(freeinode < NINODES);
    bzero(&din, sizeof(din));
    din.type = xshort(type);
    din.nlink = xshort(1);
    din.size = xint(0);
    winode(inum, &din);
    return inum;
}

// Update the free block list by marking some blocks as in-use
void balloc(int used)
{
    uchar buf[BSIZE];
    int i;

    printf("balloc: first %d blocks have been allocated\n", used);
    assert(used < BSIZE * 8);
    bzero(buf, BSIZE);
    for (i = 0; i < used; i++)
    {
        buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
    }
    printf("balloc: write bitmap block at sector %d\n", sbs[current_partition].bmapstart + sbs[current_partition].offset + 1);
    wsect(sbs[current_partition].bmapstart, buf, 0);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

// Append more data to the file with i-node number inum
void iappend(uint inum, void *xp, int n)
{
    char *p = (char *)xp;
    uint fbn, off, n1;
    struct dinode din;
    char buf[BSIZE];
    uint indirect[NINDIRECT];
    uint x;

    rinode(inum, &din);
    off = xint(din.size);
    while (n > 0)
    {
        fbn = off / BSIZE;
        assert(fbn < MAXFILE);
        if (fbn < NDIRECT)
        {
            if (xint(din.addrs[fbn]) == 0)
            {
                din.addrs[fbn] = xint(freeblock++);
            }
            x = xint(din.addrs[fbn]);
        }
        else
        {
            if (xint(din.addrs[NDIRECT]) == 0)
            {
                din.addrs[NDIRECT] = xint(freeblock++);
            }
            rsect(xint(din.addrs[NDIRECT]), (char *)indirect, 0);
            if (indirect[fbn - NDIRECT] == 0)
            {
                indirect[fbn - NDIRECT] = xint(freeblock++);
                wsect(xint(din.addrs[NDIRECT]), (char *)indirect, 0);
            }
            x = xint(indirect[fbn - NDIRECT]);
        }
        n1 = min(n, (fbn + 1) * BSIZE - off);
        rsect(x, buf, 0);
        bcopy(p, buf + off - (fbn * BSIZE), n1);
        wsect(x, buf, 0);
        n -= n1;
        off += n1;
        p += n1;
    }
    assert(freeblock < FSSIZE);
    din.size = xint(off);
    winode(inum, &din);
}

// Add the given filename and i-number as a directory entry
void dappend(int dirino, char *name, int fileino)
{
    struct xv6dirent de;

    bzero(&de, sizeof(de));
    de.inum = xshort(fileino);
    strncpy(de.name, name, DIRSIZ);
    iappend(dirino, &de, sizeof(de));
}

// Add a file to the directory with given i-num
void fappend(int dirino, char *filename, struct stat *sb)
{
    char buf[BSIZE];
    int cc, fd, inum;

    // Open the file up
    if ((fd = open(filename, 0)) < 0)
    {
        perror(filename);
        exit(1);
    }

    // Allocate an i-node for the file
    inum = ialloc(T_FILE, sb->st_mtime);

    // Add the file's name to the root directory
    dappend(dirino, filename, inum);

    // Read the file's contents in and write to the filesystem
    while ((cc = read(fd, buf, sizeof(buf))) > 0)
        iappend(inum, buf, cc);

    close(fd);
}

// Given a local directory name and a directory i-node number
// on the image, add all the files from the local directory
// to the on-image directory
void add_directory(int dirino, char *localdir)
{
    DIR *D;
    struct dirent *dent;
    struct stat sb;
    int newdirino;

    D = opendir(localdir);
    if (D == NULL)
    {
        perror(localdir);
        exit(1);
    }
    chdir(localdir);

    while ((dent = readdir(D)) != NULL)
    {

        // Skip . and ..
        if (!strcmp(dent->d_name, "."))
            continue;
        if (!strcmp(dent->d_name, ".."))
            continue;

        if (stat(dent->d_name, &sb) == -1)
        {
            perror(dent->d_name);
            exit(1);
        }

        if (S_ISDIR(sb.st_mode))
        {
            newdirino = makdir(dirino, dent->d_name, &sb);
            add_directory(newdirino, dent->d_name);
        }
        if (S_ISREG(sb.st_mode))
        {
            fappend(dirino, dent->d_name, &sb);
        }
    }

    closedir(D);
    chdir("..");
}

// Make a directory entry in the directory with the given i-node number
// and return the new directory's i-number
int makdir(int dirino, char *newdir, struct stat *sb)
{
    int ino;

    // Allocate the inode number for this directory
    // and set up the . and .. entries
    ino = ialloc(T_DIR, sb->st_mtime);
    dappend(ino, ".", ino);
    dappend(ino, "..", dirino);

    // In the parent directory, add the new directory entry
    dappend(dirino, newdir, ino);

    return (ino);
}
