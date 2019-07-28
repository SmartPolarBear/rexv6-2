#if !defined(__INCLUDE_XV6_BUDDY_H)
#define __INCLUDE_XV6_BUDDY_H

#include "xv6/types.h"
#if defined(__cplusplus)
extern "C"
{
#endif

void buddy_init(void *vstart, void *vend);

void *kmalloc(int order);
void kmfree(void *mem, int order);

void *kmalloc2(uint32_t size);
void kmfree2(void *v);

void free_page(void *v);
void *alloc_page(void);

int get_order(uint32_t v);

#if defined(__cplusplus)
}
#endif

#endif // __INCLUDE_XV6_BUDDY_H
