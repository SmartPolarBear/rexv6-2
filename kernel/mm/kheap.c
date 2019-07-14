#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/kheap.h"
#include "xv6/mmu.h"
#include "xv6/memlayout.h"

extern pde_t *kpgdir;
static kheap_header_t *heap_first = NULL;
static uint heap_top = KHEAP_START;

static void split_chunk(kheap_header_t *header, uint len);
static void alloc_chunk(uint chunk, uint len);
static void glue_chunk(kheap_header_t *chunk);

static void split_chunk(kheap_header_t *header, uint len)
{
    if (header->len - len <= sizeof(kheap_header_t))
        return;

    kheap_header_t *chunk_next = (kheap_header_t *)((uint)header + header->len);
    chunk_next->len = header->len - len;
    chunk_next->prev = header;
    chunk_next->next = header->next;
    chunk_next->allocated = 0;
    header->next = chunk_next;
    header->len = len;
}

static void alloc_chunk(uint chunk, uint len)
{
    while (chunk + len > heap_top)
    {
        uint page = (uint)kalloc();
        if (!page)
            panic("kheap:can't kalloc.\n");

        mmap(kpgdir, heap_top, V2P(page), PTE_W | PTE_P | PTE_U);
        heap_top += PGSIZE;
    }
}

static void glue_chunk(kheap_header_t *chunk)
{
    if (chunk->next && chunk->allocated == 0)
    {
        chunk->len += chunk->next->len;
        chunk->next = chunk->next->next;
        if (chunk->next->next)
        {
            chunk->next->next->prev = chunk;
        }
    }

    if (chunk->prev && chunk->allocated == 0)
    {
        chunk->prev->len += chunk->len;
        chunk->prev->next = chunk->next;
        if (chunk->next)
        {
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    if (chunk->next == NULL)
    {
        if (chunk->prev == NULL)
            heap_first = NULL;
        else
            chunk->prev->next = NULL;

        while (heap_top - PGSIZE >= (uint)chunk)
        {
            heap_top -= PGSIZE;
            uint p;
            getmapping(kpgdir, heap_top, &p);
            kfree((uint8_t *)P2V(p));
            unmap(kpgdir, heap_top);
        }
    }
}

void kmfree(void *p)
{
    cprintf("chunk addr=%d\n", (int)((uint)p - sizeof(kheap_header_t)) >= 0);
    kheap_header_t *chunk = (kheap_header_t *)((uint)p - sizeof(kheap_header_t));
    chunk->allocated = 0;
    cprintf("fuck");
    glue_chunk(chunk);
}

void *kmalloc(uint len)
{
    len += sizeof(kheap_header_t);
    kheap_header_t *cur_header = heap_first;
    kheap_header_t *prev_header = NULL;

    while (cur_header)
    {
        if (cur_header->allocated == 0 && cur_header->len >= len)
        {
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            return (void *)((uint)cur_header + sizeof(kheap_header_t));
        }
        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    uint chunk_start;
    if (!prev_header)
        chunk_start = KHEAP_START;
    else
        chunk_start = (uint)prev_header + prev_header->len;
    alloc_chunk(chunk_start, len);

    uint i;
    cur_header = (kheap_header_t *)chunk_start;

    cur_header->allocated = 1;
    cur_header->len += len;
    cur_header->next = NULL;
    cur_header->prev = prev_header;

    if (prev_header)
        prev_header->next = cur_header;
    if (heap_first == NULL)
        heap_first = cur_header;

    return (void *)(chunk_start + sizeof(kheap_header_t));
}