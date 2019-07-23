/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-23 14:04:26
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

typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef int BOOL;

#define NULL ((void *)(0))
#define TRUE ((BOOL)1)
#define FALSE ((BOOL)0)

#if !defined(NONKERNEL)

/* Explicitly-sized versions of integer types */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

/* *
 * Pointers and addresses are 32 bits long.
 * We use pointer types to represent addresses,
 * uintptr_t to represent the numerical values of addresses.
 * */
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

/* size_t is used for memory object sizes */
typedef uintptr_t size_t;

#endif // NONKERNEL

#endif