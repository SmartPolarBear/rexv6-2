// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/spinlock.h"

#define offsetof(type, member) (size_t) & (((type *)0)->member)
#define container_of(ptr, type, member) ({ \
                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                (type *)( (char *)__mptr - offsetof(type,member) ); })

extern char end[]; // first address after kernel loaded from ELF file

void freerange(void *vstart, void *vend);

typedef struct run
{
    struct run *next;
    char block[0];
} run_t;

struct
{
    spinlock_t lock;
    run_t *freelist;
    bool use_lock;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart, void *vend)
{
    initlock(&kmem.lock, "kmem");
    kmem.use_lock = false;
    freerange(vstart, vend);
}

void kinit2(void *vstart, void *vend)
{
    freerange(vstart, vend);
    kmem.use_lock = true;
}

void freerange(void *vstart, void *vend)
{
    char *p;
    p = (char *)PGROUNDUP((uint)vstart);
    for (; p + PGSIZE <= (char *)vend; p += PGSIZE)
        kfree(p);
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(char *v)
{
    struct run *r;
    if ((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);

    if (kmem.use_lock)
        acquire(&kmem.lock);

    r = (run_t *)(v);
    r->next = kmem.freelist;
    kmem.freelist = r;

    if (kmem.use_lock)
        release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char *
kalloc(void)
{
    struct run *r;

    if (kmem.use_lock)
        acquire(&kmem.lock);

    r = kmem.freelist;
    if (r)
        kmem.freelist = r->next;

    if (kmem.use_lock)
        release(&kmem.lock);

    return (char *)r;
}
