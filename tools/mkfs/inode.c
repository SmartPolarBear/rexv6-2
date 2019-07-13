#include "mkfs.h"

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