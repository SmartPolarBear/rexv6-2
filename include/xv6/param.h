/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-29 13:52:05
 * @ Description:
 */

#if !defined(__INCLUDE_XV6_PARAM_H)
#define __INCLUDE_XV6_PARAM_H

#define NPROC        32  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define MDEV         30  // minimum major device number
#define ROOTDEV       0  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
// #define FSSIZE       100000  // size of file system in blocks
//#define PARTSIZE     25000

#define NLOCK        64  // maximum number of lock
#define NSEM         64  // maximum number of semaphore
#define NPNODE       NPROC  // maximum number of process node
#define NRW	     64  // maximum number of rwlock


//shouldn't be included by the .S files
#if !defined(__ASSEMBLER__)

#include "xv6/mbr.h"
#define FSMAXSIZE    (100000) 
#define FSSIZE       ((FSMAXSIZE) / (NPARTITIONS))  // size of file system in blocks

#endif // __ASSEMBLER__


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


#endif // __INCLUDE_XV6_PARAM_H
