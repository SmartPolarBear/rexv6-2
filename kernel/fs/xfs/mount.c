/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-02 17:17:08
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-29 21:18:35
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/fs.h"
#include "xv6/mount.h"
#include "xv6/file.h"

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

    if (ip->major = NDEVHDAP)
    {
        if (ip->minor < 0 || ip->minor > 3)
        {
            cprintf("mount: partition number %d out of bounds\n", ip->minor);
            return -1;
        }
        if ((ip = namei(target)) == 0)
        {
            cprintf("mount: path not found\n");
            return -1;
        }
        return insert_mapping(ip, ip->minor);
    }
}

int unmount(int dev)
{
}
