#ifndef __INCLUDE_XV6_TYPES_H
#define __INCLUDE_XV6_TYPES_H

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

typedef int userlock;
typedef int usersem;
typedef int userrwlock;

typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef int BOOL;

#define NULL ((void*)(0))
#define TRUE ((BOOL)1)
#define FALSE ((BOOL)0)
#endif