#include "inode.h"
#include "byte_order.h"
#include "sect.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cassert>

#include "block.h"

using std::min;

using block::freeblock;

using byte_order_intel::xint;
using byte_order_intel::xshort;

void inode::winode(struct superblock &sb, uint inum, struct dinode *ip)
{
    char buf[BSIZE];
    uint bn;
    struct dinode *dip;

    bn = IBLOCK(inum, sb);
    sector::rsect(bn, buf);
    dip = ((struct dinode *)buf) + (inum % IPB);
    *dip = *ip;
    sector::wsect(bn, buf);
}

void inode::rinode(struct superblock &sb, uint inum, struct dinode *ip)
{
    char buf[BSIZE];
    uint bn;
    struct dinode *dip;

    bn = IBLOCK(inum, sb);
    sector::rsect(bn, buf);
    dip = ((struct dinode *)buf) + (inum % IPB);
    *ip = *dip;
}

void inode::iappend(struct superblock &sb, uint inum, void *xp, int n)
{
    char *p = (char *)xp;
    uint fbn, off, n1;
    struct dinode din;
    char buf[BSIZE];
    uint indirect[NINDIRECT];
    uint x;

    rinode(sb, inum, &din);
    off = xint(din.size);
    // printf("append inum %d at off %d sz %d\n", inum, off, n);
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
            sector::rsect(xint(din.addrs[NDIRECT]), (char *)indirect);
            if (indirect[fbn - NDIRECT] == 0)
            {
                indirect[fbn - NDIRECT] = xint(freeblock++);
                sector::wsect(xint(din.addrs[NDIRECT]), (char *)indirect);
            }
            x = xint(indirect[fbn - NDIRECT]);
        }
        n1 = min((uint)n, (fbn + 1) * BSIZE - off);
        sector::rsect(x, buf);
        bcopy(p, buf + off - (fbn * BSIZE), n1);
        sector::wsect(x, buf);
        n -= n1;
        off += n1;
        p += n1;
    }
    din.size = xint(off);
    winode(sb, inum, &din);
}

uint inode::ialloc(struct superblock &sb, ushort type)
{
    uint inum = freeinode++;
    struct dinode din;

    bzero(&din, sizeof(din));
    din.type = xshort(type);
    din.nlink = xshort(1);
    din.size = xint(0);
    winode(sb, inum, &din);
    return inum;
}
