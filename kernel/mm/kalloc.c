/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-24 15:16:06
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-07-24 15:21:50
 * @ Description:
 */

// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/spinlock.h"
#include "xv6/buddy.h"

extern char end[]; // first address after kernel loaded from ELF file

void freerange(void *vstart, void *vend);

typedef enum allocator_type
{
    FREELIST, //freelist
    BUDDY     //buddy
} allocator_type_t;

typedef struct run
{
    struct run *next;
} run_t;

typedef struct
{
    spinlock_t lock;
    run_t *freelist;
    allocator_type_t type;
} kmem_t;

static kmem_t kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart, void *vend)
{
    initlock(&kmem.lock, "kmem");
    kmem.type = FREELIST;

    for (char *p = (char *)PGROUNDUP((uint)vstart); p + PGSIZE <= (char *)vend; p += PGSIZE)
        kfree(p);
}

void kinit2(void *vstart, void *vend)
{
    // freerange(vstart, vend);
    buddy_init();
    buddy_init2(vstart, vend);
    kmem.type = BUDDY;
}

static void fl_free(char *v)
{

    struct run *r;
    if ((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);

    r = (run_t *)(v);
    r->next = kmem.freelist;
    kmem.freelist = r;
}

static char *fl_kalloc(void)
{
    struct run *r;

    r = kmem.freelist;
    if (r)
        kmem.freelist = r->next;

    return (char *)r;
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(char *v)
{
    if (kmem.type == BUDDY)
    {
        free_page(v);
    }
    else if (kmem.type == FREELIST)
    {
        fl_free(v);
    }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char *
kalloc(void)
{
    if (kmem.type == BUDDY)
    {
        return alloc_page();
    }
    else if (kmem.type == FREELIST)
    {
        return fl_kalloc();
    }
}
