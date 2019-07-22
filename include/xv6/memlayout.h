/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-18 23:34:10
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-22 20:41:19
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

#endif // __INCLUDE_XV6_MEMLAYOUT_H
