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

void stdfreerange(void *vstart, void *vend);

struct run
{
    struct run *next;
};

enum ALLOCATORTYPE
{
    STD,
    BUDDY
};

struct
{
    struct spinlock lock;
    int use_lock;
    int type;
    buddy_list_bucket_t *buddy;
    struct run *freelist;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart, void *vend)
{
    initlock(&kmem.lock, "kmem");
    kmem.use_lock = FALSE;
    kmem.type = STD;
    stdfreerange(vstart, vend);
}

void kinit2(void *vstart, void *vend)
{
    // stdfreerange(vstart, vend);
    buddyinit(vstart, vend);
    kmem.use_lock = TRUE;
    kmem.type = BUDDY;
    cprintf("switch to buddy.\n");
}

void stdfreerange(void *vstart, void *vend)
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
void stdkfree(char *v)
{
    struct run *r;

    if ((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);

    if (kmem.use_lock)
        acquire(&kmem.lock);

    r = (struct run *)v;
    r->next = kmem.freelist;
    kmem.freelist = r;

    if (kmem.use_lock)
        release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char *stdkalloc(void)
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

int __log2(int v)
{
    int r = 0xFFFF - v >> 31 & 0x10;
    v >>= r;
    int shift = 0xFF - v >> 31 & 0x8;
    v >>= shift;
    r |= shift;
    shift = 0xF - v >> 31 & 0x4;
    v >>= shift;
    r |= shift;
    shift = 0x3 - v >> 31 & 0x2;
    v >>= shift;
    r |= shift;
    r |= (v >> 1);
    return r;
}

void buddyinit(void *bg, void *ed)
{
    if (kmem.use_lock)
        acquire(&kmem.lock);

    unsigned size = ed - bg;
    cprintf("init at 0x%x,end at 0x%x,size %d\n", (int)bg, (int)(ed), (int)size);

    kmem.buddy = create_buddy_table(bg, __log2(size));

    if (kmem.use_lock)
        release(&kmem.lock);
}

char *kalloc(void)
{
    if (kmem.type == STD)
    {
        return stdkalloc();
    }
    else
    {
        void *p = buddy_alloc(kmem.buddy, PGSIZE);
        cprintf("p is at 0x%x\n",(int)p);
        return p;
    }
}

void kfree(char *p)
{
    if (kmem.type == STD)
    {
        stdkfree(p);
    }
    else
    {
        cprintf("fuck2\n");
        buddy_free(p);
    }
}