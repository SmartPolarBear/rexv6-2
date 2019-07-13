#include "mkfs.h"

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