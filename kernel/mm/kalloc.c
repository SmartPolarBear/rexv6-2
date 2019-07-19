// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/spinlock.h"

extern char end[]; // first address after kernel loaded from ELF file

typedef struct buddy
{
    unsigned size;
    unsigned longest[1];
} buddy_t;

struct
{
    struct spinlock lock;
    BOOL use_lock;
    buddy_t *buddy;
} kmem;

#define POWER_OF_2(x) (((x) != 0) && !((x) & ((x)-1)))

void buddyinit(void *vstart, void *vend)
{
    unsigned size = vend - vstart, nodesize = 0;
    if (size < 1 || !POWER_OF_2(2))
        panic("buddyinit");

    if (kmem.use_lock)
        acquire(&kmem.lock);

    kmem.buddy = (buddy_t *)vstart;
    kmem.buddy->size = size;
    nodesize = 2 * size;

    for (int i = 0; i < 2 * size - 1; i++)
    {
        if (POWER_OF_2(i + 1))
            nodesize /= 2;

        kmem.buddy->longest[i] = nodesize;
    }

    if (kmem.use_lock)
        release(&kmem.lock);
}

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart, void *vend)
{
    initlock(&kmem.lock, "kmem");
    kmem.use_lock = FALSE;
    buddyinit(vstart, vend);
}

void kinit2(void *vstart, void *vend)
{
    buddyinit(vstart, vend);
    kmem.use_lock = TRUE;
}

#define LEFTCHILD(index) ((index)*2)
#define RIGHTCHILD(index) ((index)*2 + 1)
#define PARENT(index) (((index) == 0) ? (0) : (((int)(floor((((float)(index)) - 1.0) / 2.0)))))

uint fixsize(uint size)
{
    unsigned int v = (unsigned int)size;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
}

offset_t buddyalloc(uint size)
{
    unsigned index = 0, nodeszie = 0, offset = 0;
    if (kmem.buddy == NULL)
        panic("kmem.buddy NULL");

    if (size <= 0)
        size = 1;
    else if (!POWER_OF_2(size))
        size = fixsize(size);

    if (kmem.buddy->longest[index] < size)
        return -1;

    for (nodeszie = kmem.buddy->size; nodeszie != size; nodeszie /= 2)
    {
        index = kmem.buddy->longest[LEFTCHILD(index)] >= size
                    ? LEFTCHILD(index)
                    : RIGHTCHILD(index);
    }

    kmem.buddy->longest[index] = 0;
    offset = (index + 1) * nodeszie - kmem.buddy->size;
    while (index)
    {
        index = PARENT(index);
        kmem.buddy->longest[index] =
            MAX(kmem.buddy->longest[LEFTCHILD(index)], kmem.buddy->longest[RIGHTCHILD(index)]);
    }

    return offset;
}

void buddyfree(offset_t offset)
{
    unsigned node_size, index = 0;
    unsigned left_longest, right_longest;

    if (kmem.buddy == NULL || offset < 0 || offset > kmem.buddy->size)
    {
        panic("error free");
    }

    node_size = 1;
    index = offset + kmem.buddy->size - 1;

    for (; kmem.buddy->longest[index]; index = PARENT(index))
    {
        node_size *= 2;
        if (index == 0)
            return;
    }

    kmem.buddy->longest[index] = node_size;

    while (index)
    {
        index = PARENT(index);
        node_size *= 2;

        left_longest = kmem.buddy->longest[LEFTCHILD(index)];
        right_longest = kmem.buddy->longest[RIGHTCHILD(index)];

        if (left_longest + right_longest == node_size)
            kmem.buddy->longest[index] = node_size;
        else
            kmem.buddy->longest[index] = MAX(left_longest, right_longest);
    }
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(char *v)
{
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char *
kalloc(void)
{
}

// void freerange(void *vstart, void *vend);

// struct run
// {
//     struct run *next;
// };

// struct
// {
//     struct spinlock lock;
//     int use_lock;
//     struct run *freelist;
// } kmem;

// // Initialization happens in two phases.
// // 1. main() calls kinit1() while still using entrypgdir to place just
// // the pages mapped by entrypgdir on free list.
// // 2. main() calls kinit2() with the rest of the physical pages
// // after installing a full page table that maps them on all cores.
// void kinit1(void *vstart, void *vend)
// {
//     initlock(&kmem.lock, "kmem");
//     kmem.use_lock = 0;
//     freerange(vstart, vend);
// }

// void kinit2(void *vstart, void *vend)
// {
//     freerange(vstart, vend);
//     kmem.use_lock = 1;
// }

// void freerange(void *vstart, void *vend)
// {
//     char *p;
//     p = (char *)PGROUNDUP((uint)vstart);
//     for (; p + PGSIZE <= (char *)vend; p += PGSIZE)
//         kfree(p);
// }

// //PAGEBREAK: 21
// // Free the page of physical memory pointed at by v,
// // which normally should have been returned by a
// // call to kalloc().  (The exception is when
// // initializing the allocator; see kinit above.)
// void kfree(char *v)
// {
//     struct run *r;

//     if ((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
//         panic("kfree");

//     // Fill with junk to catch dangling refs.
//     memset(v, 1, PGSIZE);

//     if (kmem.use_lock)
//         acquire(&kmem.lock);

//     r = (struct run *)v;
//     r->next = kmem.freelist;
//     kmem.freelist = r;

//     if (kmem.use_lock)
//         release(&kmem.lock);
// }

// // Allocate one 4096-byte page of physical memory.
// // Returns a pointer that the kernel can use.
// // Returns 0 if the memory cannot be allocated.
// char *
// kalloc(void)
// {
//     struct run *r;

//     if (kmem.use_lock)
//         acquire(&kmem.lock);
//     r = kmem.freelist;

//     if (r)
//         kmem.freelist = r->next;

//     if (kmem.use_lock)
//         release(&kmem.lock);
//     return (char *)r;
// }
