#if !defined(__INCLUDE_XV6_MOUNT_H)
#define __INCLUDE_XV6_MOUNT_H

int mount(char *src, char *target,int fs);
int unmount(int dev);

#endif // __INCLUDE_XV6_MOUNT_H
