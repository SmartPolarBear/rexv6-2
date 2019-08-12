#if !defined(__INCLUDE_XV6_FREELIST_H)
#define __INCLUDE_XV6_FREELIST_H

#include "xv6/types.h"
#if defined(__cplusplus)
extern "C"
{
#endif

void freelist_init(void *vstart, void *vend);
void freelist_free(char *v);
char *freelist_alloc(void);

#if defined(__cplusplus)
}
#endif

#endif // __INCLUDE_XV6_FREELIST_H
