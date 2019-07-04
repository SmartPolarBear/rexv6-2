/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-02 17:19:42
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-04 18:26:43
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/stat.h"
#include "xv6/fs.h"
#include "xv6/mount.h"

int sys_mount(void)
{
    char *src, *target;
    argstr(0, &src);
    argstr(1, &target);
    mount(src, target, XV6FS);
    return 0;
}

int sys_unmount(void)
{
    int dev;
    argint(0, &dev);
    unmount(dev);
    return 0;
}
