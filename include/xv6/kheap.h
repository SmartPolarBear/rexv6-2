#if !defined(__INCLUDE_XV6_KHEAP_H)
#define __INCLUDE_XV6_KHEAP_H

#include "xv6/types.h"

#define KHEAP_START (0xA0000000)

typedef struct kheap_header
{
    struct kheap_header *prev, *next;
    BOOL allocated : 1;
    uint len : 31;
} kheap_header_t;

void kheapinit();

void *kmalloc(uint len);
void kmfree(void *p);

#endif // __INCLUDE_XV6_KHEAP_H
