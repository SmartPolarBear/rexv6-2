#if !defined(__TOOLS_MKFS_COMMON_H)
#define __TOOLS_MKFS_COMMON_H

#include "xv6/types.h"

#if !defined(stat)
#define stat xv6_stat // avoid clash with host struct stat
#endif // stat

#include "xv6/types.h"
#include "xv6/fs.h"
#include "xv6/stat.h"
#include "xv6/param.h"

#endif // __TOOLS_MKFS_COMMON_H
