/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-17 23:27:46
 * @ Description:
 */


#ifndef __INCLUDE_XV6_TYPES_H
#define __INCLUDE_XV6_TYPES_H

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
typedef uint offset_t;

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