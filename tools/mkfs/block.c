#include "mkfs.h"

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
