/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-02 17:17:08
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-29 14:02:45
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/fs.h"
#include "xv6/mount.h"
#include "xv6/file.h"

struct mountsw mountsw[NDEV];
struct mountsw *mntswend;
struct fstable fstable[NDEV];

int regfs(int fsid, struct fstable *fs)
{
    fstable[fsid] = *fs;
    return 0;
}

struct fstable *
getfs(int fsid)
{
    return fstable + fsid;
}

int mountdev(int dev, char *path, int fs)
{
    cprintf("mountdev(%d,%s,XV6FS).\n", dev, path);
    if (mntswend - mountsw >= NDEV)
        return -1;
    mntswend->dev = dev;
    mntswend->dp = namei(path);
    mntswend->fsid = fs;
    mntswend++;
    return 0;
}

int mountpart(char *path, uint partition_number)
{
    cprintf("mountpart(%s,%d).\n", path, partition_number);
    struct inode *ip;
    if (partition_number < 0 || partition_number > 3)
    {
        cprintf("mountpart: partition number out of bounds\n");
        return -1;
    }
    if ((ip = namei(path)) == 0)
    {
        cprintf("mountpart: path not found\n");
        return -1;
    }
    cprintf("type=%d.", ip->type);
    return insert_mapping(ip, partition_number);
}

_Bool ispartition(struct inode *ip)
{
    if (ip->major != NDEVHDA)
        return false;

    int part = ip->minor - 3;

    return part >= 0 && part <= 3;
}

int mount(char *src, char *target, int fs)
{
    begin_op();
    struct inode *ip = namei(src);
    if (ip == 0)
    {
        end_op();
        return -1;
    }

    ilock(ip);
    int type = ip->type, minor = ip->minor, dev = ip->dev, inum = ip->inum;
    iunlockput(ip);
    end_op();

    if (type == T_DEV)
    {
        if (ispartition(ip))
        {
            cprintf("ispartition,inum=%d\n", inum);
            mountpart(target, minor - 3);
        }
        else
        {
            cprintf("not ispartition\n");
            mountdev(dev, target, fs);
        }
    }
}

int unmount(int dev)
{
    struct mountsw *mp;
    for (mp = mountsw; mp != mntswend; mp++)
    {
        if (mp->dev == dev)
        {
            *mp = *(--mntswend);
            return 0;
        }
    }
    return -1;
}

//this mounts the root device
void mountinit(void)
{
    mntswend = mountsw;
    struct fstable deffs = {deffsread, deffswrite};
    regfs(XV6FS, &deffs);
    mountdev(ROOTDEV, "/", XV6FS);
}
