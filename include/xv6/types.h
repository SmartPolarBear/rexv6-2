/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-24 23:50:05
 * @ Description:
 */

#ifndef __INCLUDE_XV6_TYPES_H
#define __INCLUDE_XV6_TYPES_H

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef uint pde_t;
typedef uint offset_t;

typedef int userlock;
typedef int usersem;
typedef int userrwlock;

#define NULL ((void*)0)

#if !defined(NONKERNEL)
typedef unsigned int size_t;

#include "stdint.h"
#include "stdbool.h"

#endif // NONKERNEL

#endif