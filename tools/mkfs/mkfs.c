/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-06 00:10:55
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-14 00:01:05
 * @ Description:
 */

#include "mkfs.h"

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

const int nbitmap = FSSIZE / (BSIZE * 8) + 1;
const int ninodeblocks = NINODES / IPB + 1;
const int nlog = LOGSIZE;

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

    _Static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

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

    freeblock = nmeta; // The first free block that we can allocate
    master_freeblock = freeblock;

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
        sbs[i].initusedblock = freeblock + 1;
    }

    printf("Each partition has the following composition: nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n", nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

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
    
    return 0;
}
