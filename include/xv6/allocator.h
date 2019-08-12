#if !defined(__INCLUDE_XV6_ALLOCATOR_H)
#define __INCLUDE_XV6_ALLOCATOR_H
#include "xv6/types.h"

typedef void (*init_handler_t)(void *vs, void *ve);
typedef char *(*fix_alloc_handler_t)(void);
typedef void (*fix_free_handler_t)(char *mem);
typedef void *(*dyn_alloc_handler_t)(uint size);
typedef void (*dyn_free_handler_t)(void *mem);

typedef struct allocator
{
    uint id;

    init_handler_t init;

    fix_alloc_handler_t fix_alloc;
    fix_free_handler_t fix_free;

    dyn_alloc_handler_t dyn_alloc;
    dyn_free_handler_t dyn_free;

    int is_dyn;
} allocator_t;

typedef enum allocator_id
{
    FREELIST,
    BUDDY
} allocator_id_t;

#endif // __INCLUDE_XV6_ALLOCATOR_H
