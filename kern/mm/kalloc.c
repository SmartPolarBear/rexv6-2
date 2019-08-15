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
