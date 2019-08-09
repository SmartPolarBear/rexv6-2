#if !defined(__TOOLS_MKFS_SECT_H)
#define __TOOLS_MKFS_SECT_H

#include "common.h"

namespace sector
{
int fsfd = -1;
void wsect(uint, void *);
void rsect(uint sec, void *buf);
} // namespace sector

#endif // __TOOLS_MKFS_SECT_H
