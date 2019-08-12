#if !defined(__INCLUDE_XV6_KALLOC_H)
#define __INCLUDE_XV6_KALLOC_H

#include "xv6/allocator.h"
#if defined(__cplusplus)
extern "C"
{
#endif
char *kalloc(void);
void kfree(char *);
void *kmalloc(size_t size);
void kmfree(void *p);
void initmem(void *vstart, void *vend);
void switch_allocator(allocator_id_t id);
#if defined(__cplusplus)
}
#endif
#endif // __INCLUDE_XV6_KALLOC_H
