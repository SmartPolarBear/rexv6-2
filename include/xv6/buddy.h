#if !defined(__INCLUDE_xV6_BUDDY_H)
#define __INCLUDE_xV6_BUDDY_H

#include "xv6/types.h"

void            kmem_init (void);
void            kmem_init2(void *vstart, void *vend);

void*           kmalloc (int order);
void            kmfree (void *mem, int order);

void            free_page(void *v);
void*           alloc_page (void);

int             get_order (uint32_t v);


#endif // __INCLUDE_xV6_BUDDY_H
