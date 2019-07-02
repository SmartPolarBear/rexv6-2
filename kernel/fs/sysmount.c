/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-02 17:19:42
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-02 23:02:25
 * @ Description:
 */


#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/fs.h"
#include "xv6/mount.h"

int
sys_mount(void)
{
    int dev, fs;
    char *path;
    argint(0, &dev);
    argstr(1, &path);
    argint(2, &fs);
    mount(dev, path, fs);
    return 0;
}

int
sys_unmount(void)
{
    int dev;
    argint(0, &dev);
    unmount(dev);
    return 0;
}
