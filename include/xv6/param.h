#if !defined(__INCLUDE_XV6_PARAM_H)
#define __INCLUDE_XV6_PARAM_H

#define NPROC 64                  // maximum number of processes
#define KSTACKSIZE 4096           // size of per-process kernel stack
#define NCPU 8                    // maximum number of CPUs
#define NOFILE 16                 // open files per process
#define NFILE 100                 // open files per system
#define NINODE 50                 // maximum number of active i-nodes
#define NDEV 10                   // maximum major device number
#define ROOTDEV 1                 // device number of file system root disk
#define MAXARG 32                 // max exec arguments
#define MAXOPBLOCKS 10            // max # of blocks any FS op writes
#define LOGSIZE (MAXOPBLOCKS * 3) // max data blocks in on-disk log
#define NBUF (MAXOPBLOCKS * 3)    // size of disk block cache
#define FSSIZE 1000               // size of file system in blocks

#if !defined(NONKERNEL) && !defined(__ASSEMBLER__)

#include "xv6/types.h"
#if !defined(offsetof)
#define offsetof(s,memb) \
    ((size_t)((char *)&((s *)0)->memb - (char *)0))
#endif //offsetof

#if !defined(container_of)
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif //container_of
#endif // NONKERNEL

#endif // __INCLUDE_XV6_PARAM_H
