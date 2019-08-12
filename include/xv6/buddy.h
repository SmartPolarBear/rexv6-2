#if !defined(__INCLUDE_XV6_BUDDY_H)
#define __INCLUDE_XV6_BUDDY_H

#include "xv6/types.h"
#if defined(__cplusplus)
extern "C"
{
#endif

void buddy_init(void *vstart, void *vend);

void *buddy_kmalloc(int order);
void buddy_kmfree(void *mem, int order);

void *buddy_kmalloc2(uint32_t size);
void buddy_kmfree2(void *v);

void buddy_free_page(char *v);
char *buddy_alloc_page(void);

int buddy_get_order(uint32_t v);

#if defined(__cplusplus)
}
#endif

#endif // __INCLUDE_XV6_BUDDY_H
