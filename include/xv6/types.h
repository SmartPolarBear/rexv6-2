#if !defined(__INCLUDE_XV6_TYPES_H)
#define __INCLUDE_XV6_TYPES_H

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef uint pde_t;

#if !defined(NONKERNEL) && !defined(__ASSEMBLER__)
typedef unsigned long size_t;

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;


#if !defined(NULL)
#define NULL ((void *)0)
#endif

#endif // NONKERNEL
#endif // __INCLUDE_XV6_TYPES_H
