/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-13 23:26:01
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-14 00:01:40
 * @ Description:
 */

#if !defined(__MKFS_MKFS_H)
#define __MKFS_MKFS_H

#define NONKERNEL

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <assert.h>

#define dirent xv6dirent
#define stat xv6stat
#include "xv6/types.h"
#include "xv6/fs.h"
#include "xv6/stat.h"
#include "xv6/param.h"
#include "xv6/mbr.h"

#undef dirent
#undef stat

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

extern const int nbitmap;
extern const int ninodeblocks;
extern const int nlog;

extern int nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
extern int nblocks; // Number of data blocks

extern int fsfd;
extern struct superblock sbs[4];
extern mbr_t mbr;
extern char zeroes[BSIZE];
extern uint freeinode;
extern uint freeblock;
extern uint master_freeblock;
extern int current_partition;
extern dpartition_t partitions[4];

void balloc(int);
void wsect(uint, void *, int);
void winode(uint, struct dinode *);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf, int);
uint ialloc(ushort type, int mtime);
void iappend(uint inum, void *p, int n);
void dappend(int dirino, char *name, int fileino);
void add_directory(int dirino, char *localdir);
int makdir(int dirino, char *newdir, struct stat *sb);

#define min(a, b) ((a) < (b) ? (a) : (b))

#endif // __MKFS_MKFS_H
