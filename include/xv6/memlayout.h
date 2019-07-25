/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-18 23:34:10
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-25 16:20:12
 * @ Description:
 */

#if !defined(__INCLUDE_XV6_MEMLAYOUT_H)
#define __INCLUDE_XV6_MEMLAYOUT_H

// Memory layout

#define EXTMEM 0x100000     // Start of extended memory
#define PHYSTOP 0xE000000   // Top physical memory
#define DEVSPACE 0xFE000000 // Other devices are at high addresses

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000          // First kernel virtual address
#define KERNLINK (KERNBASE + EXTMEM) // Address where kernel is linked
#define STACKBASE (KERNBASE - 1)

#define V2P(a) (((uint)(a)) - KERNBASE)
#define P2V(a) (((void *)(a)) + KERNBASE)

#define V2P_WO(x) ((x)-KERNBASE)   // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE) // same as P2V, but without casts

//shouldn't be included by the .S files
#if !defined(__ASSEMBLER__)
#include "xv6/types.h"

// some constants for bios interrupt 15h AX = 0xE820
#define E820MAX (20) // number of entries in E820MAP
#define E820_ARM (1) // address range memory
#define E820_ARR (2) // address range reserved

typedef struct e820map
{
    int nr_map;
    struct
    {
        uint64_t addr;
        uint64_t size;
        uint32_t type;
    } __attribute__((packed)) map[E820MAX];
} e820map_t;

#endif // __ASSEMBLER__

#endif // __INCLUDE_XV6_MEMLAYOUT_H
