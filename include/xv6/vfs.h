#if !defined(__INCLUDE_XV6_VFS_H)
#define __INCLUDE_XV6_VFS_H
#include "xv6/file.h"


int vfs_get_root(const char *devname, inode_t **node_store);
int vfs_get_curdir(inode_t **dir_store);
inode_t* vfs_lookup(char *path);
inode_t* vfs_lookup_parent(char *path, char *name);

int namecmp(const char *s, const char *t);

#endif // __INCLUDE_XV6_VFS_H
