// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/spinlock.h"

#include "xv6/allocator.h"
#include "xv6/buddy.h"
#include "xv6/freelist.h"

#define NALLOCATOR (4)

allocator_t allocators[NALLOCATOR] =
    {
        [FREELIST] = {
            FREELIST,
            freelist_init,
            freelist_alloc,
            freelist_free,
            NULL,
            NULL,
            0},
        [BUDDY] = {BUDDY, buddy_init, buddy_alloc_page, buddy_free_page, buddy_kmalloc2, buddy_kmfree2, 1}};

uint current_allocator = FREELIST;

void switch_allocator(allocator_id_t id)
{
  current_allocator = id;
}

void initmem(void *vstart, void *vend)
{
  cprintf("vstart=%d,vend=%d\n", (int)vstart, (int)vstart);

  allocators[current_allocator].init(vstart, vend);
}

char *kalloc(void)
{
  return allocators[current_allocator].fix_alloc();
}

void kfree(char *p)
{
  allocators[current_allocator].fix_free(p);
}

void *kmalloc(size_t size)
{
  if (!allocators[current_allocator].is_dyn)
  {
    panic("Current allocator doesn't support dynamic size.");
  }

  return allocators[current_allocator].dyn_alloc(size);
}

void kmfree(void *p)
{
  if (!allocators[current_allocator].is_dyn)
  {
    panic("Current allocator doesn't support dynamic size.");
  }

  allocators[current_allocator].dyn_free(p);
}

// static void freerange(void *vstart, void *vend);
// extern char end[]; // first address after kernel loaded from ELF file
//                    // defined by the kernel linker script in kernel.ld

// struct run
// {
//   struct run *next;
// };

// struct
// {
//   struct spinlock lock;
//   int use_lock;
//   struct run *freelist;
// } kmem;

// // Initialization happens in two phases.
// // 1. main() calls kinit1() while still using entrypgdir to place just
// // the pages mapped by entrypgdir on free list.
// // 2. main() calls kinit2() with the rest of the physical pages
// // after installing a full page table that maps them on all cores.
// void kinit1(void *vstart, void *vend)
// {
//   initlock(&kmem.lock, "kmem");
//   kmem.use_lock = 0;
//   freerange(vstart, vend);
// }

// void kinit2(void *vstart, void *vend)
// {
//   freerange(vstart, vend);
//   kmem.use_lock = 1;
// }

// static void freerange(void *vstart, void *vend)
// {
//   char *p;
//   p = (char *)PGROUNDUP((uint)vstart);
//   for (; p + PGSIZE <= (char *)vend; p += PGSIZE)
//     kfree(p);
// }
// //PAGEBREAK: 21
// // Free the page of physical memory pointed at by v,
// // which normally should have been returned by a
// // call to kalloc().  (The exception is when
// // initializing the allocator; see kinit above.)
// void kfree(char *v)
// {
//   struct run *r;

//   if ((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
//     panic("kfree");

//   // Fill with junk to catch dangling refs.
//   memset(v, 1, PGSIZE);

//   if (kmem.use_lock)
//     acquire(&kmem.lock);
//   r = (struct run *)v;
//   r->next = kmem.freelist;
//   kmem.freelist = r;
//   if (kmem.use_lock)
//     release(&kmem.lock);
// }

// // Allocate one 4096-byte page of physical memory.
// // Returns a pointer that the kernel can use.
// // Returns 0 if the memory cannot be allocated.
// char *
// kalloc(void)
// {
//   struct run *r;

//   if (kmem.use_lock)
//     acquire(&kmem.lock);
//   r = kmem.freelist;
//   if (r)
//     kmem.freelist = r->next;
//   if (kmem.use_lock)
//     release(&kmem.lock);
//   return (char *)r;
// }
