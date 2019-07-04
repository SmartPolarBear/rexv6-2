/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-02 17:17:08
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-03 23:05:21
 * @ Description:
 */


#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/fs.h"
#include "xv6/mount.h"

struct mountsw mountsw[NDEV];
struct mountsw *mntswend;
struct fstable fstable[NDEV];

int
regfs(int fsid, struct fstable *fs)
{
    fstable[fsid] = *fs;
    return 0;
}

struct fstable*
getfs(int fsid)
{
    return fstable + fsid;
}

int
mount(int dev, char *path, int fs)
{
    if (mntswend - mountsw >= NDEV)
        return -1;
    mntswend->dev = dev;
    mntswend->dp = namei(path);
    mntswend->fsid = fs;
    mntswend++;
    return 0;
}

int unmount(int dev)
{
    struct mountsw *mp;
    for (mp = mountsw; mp != mntswend; mp++)
        if (mp->dev == dev)
        {
            *mp = *(--mntswend);
            return 0;
        }
    return -1;
}

//this mounts the root device
void
mountinit(void)
{
    mntswend = mountsw;
    struct fstable deffs = { deffsread, deffswrite };
    regfs(XV6FS, &deffs);
    mount(ROOTDEV, "/", XV6FS);
}
