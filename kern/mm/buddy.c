/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-07-24 15:03:17
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-08-15 23:25:10
 * @ Description: Buddy memory allocator
 * 
 *  this file implement the buddy memory allocator. Each order divides
 *  the memory pool into equal-sized blocks (2^n). We use bitmap to record
 *  allocation status for each block. This allows for efficient merging
 *  when blocks are freed. We also use double-linked list to chain together
 *  free blocks (for each order), thus allowing fast allocation. There is
 *  about 8% overhead (maximum) for this structure.
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/spinlock.h"
#include "xv6/buddy.h"

#define MAX_ORD (12)
#define MIN_ORD (6)
#define N_ORD (MAX_ORD - MIN_ORD + 1)

// ALIGN_UP/DOWN: al must be of power of 2
#define ALIGN_UP(sz, al) (((uint)(sz) + (uint)(al)-1) & ~((uint)(al)-1))
#define ALIGN_DOWN(sz, al) ((uint)(sz) & ~((uint)(al)-1))

// lnks is a combination of previous link (index) and next link (index)
#define PRE_LNK(lnks) ((lnks) >> 16)
#define NEXT_LNK(lnks) ((lnks)&0xFFFF)
#define LNKS(pre, next) (((pre) << 16) | ((next)&0xFFFF))
#define NIL ((uint16_t)0xFFFF)

//the order for a page-sized block.
#define PGSIZE_ORDER (12)

typedef struct mark
{
    uint32_t lnks;   // double links (actually indexes)
    uint32_t bitmap; // bitmap, whether the block is available (1=available)
} mark_t;

typedef struct order
{
    uint32_t head;   // the first non-empty mark
    uint32_t offset; // the first mark
} order_t;

typedef struct mblock
{
    int order;
    char blocks[0];
} mblock_t;

static struct
{
    struct spinlock lock;
    uint start;      // start of memory for marks
    uint start_heap; // start of allocatable memory
    uint end;
    int initialized;            //whether kmem for buddy is initialized
    struct order orders[N_ORD]; // orders used for buddy systems
} kmem;

// coversion between block id to mark and memory address
static inline struct mark *get_mark(int order, int idx)
{
    return (struct mark *)kmem.start + (kmem.orders[order - MIN_ORD].offset + idx);
}

static inline void *blkid2mem(int order, int blkid)
{
    return (void *)(kmem.start_heap + (1 << order) * blkid);
}

static inline int mem2blkid(int order, void *mem)
{
    return ((uint)mem - kmem.start_heap) >> order;
}

static inline int available(uint bitmap, int blk_id)
{
    return bitmap & (1 << (blk_id & 0x1F));
}

// round up power of 2, then get the order
//   http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
static inline uint32_t roundup_powerof2(uint32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}


void buddy_init(void *vstart, void *vend)
{
    initlock(&kmem.lock, "buddy");

    kmem.start = (uint)vstart;
    kmem.end = (uint)vend;
    uint len = kmem.end - kmem.start;

    // reserved memory at vstart for an array of marks (for all the orders)
    uint n = (len >> (MAX_ORD + 5)) + 1; // estimated # of marks for max order
    uint total = 0;
    for (int i = N_ORD - 1; i >= 0; i--)
    {
        order_t *ord = kmem.orders + i;
        ord->offset = total;
        ord->head = NIL;

        // set the bitmaps to mark all blocks not available
        for (uint j = 0; j < n; j++)
        {
            mark_t *mk = get_mark(i + MIN_ORD, j);
            mk->lnks = LNKS(NIL, NIL);
            mk->bitmap = 0;
        }

        total += n;
        n <<= 1; // each order doubles required marks
    }

    // add all available memory to the highest order bucket
    kmem.start_heap = ALIGN_UP(kmem.start + total * sizeof(mark_t), 1 << MAX_ORD);

    for (uint i = kmem.start_heap; i < kmem.end; i += (1 << MAX_ORD))
    {
        //TODO: FIX aquiring lock makes this really slow.
        buddy_kmfree((void *)i, MAX_ORD);
    }

    kmem.initialized = 1;
}

// mark a block as unavailable
static void unmark_blk(int order, int blk_id)
{
    struct mark *mk, *p;
    struct order *ord;
    int prev, next;

    ord = &kmem.orders[order - MIN_ORD];
    mk = get_mark(order, blk_id >> 5);

    // clear the bit in the bitmap
    if (!available(mk->bitmap, blk_id))
    {
        panic("double alloc\n");
    }

    mk->bitmap &= ~(1 << (blk_id & 0x1F));

    // if it's the last block in the bitmap, delete from the list
    if (mk->bitmap == 0)
    {
        blk_id >>= 5;

        prev = PRE_LNK(mk->lnks);
        next = NEXT_LNK(mk->lnks);

        if (prev != NIL)
        {
            p = get_mark(order, prev);
            p->lnks = LNKS(PRE_LNK(p->lnks), next);
        }
        else if (ord->head == blk_id)
        {
            // if we are the first in the link
            ord->head = next;
        }

        if (next != NIL)
        {
            p = get_mark(order, next);
            p->lnks = LNKS(prev, NEXT_LNK(p->lnks));
        }

        mk->lnks = LNKS(NIL, NIL);
    }
}

// mark a block as available
static void mark_blk(int order, int blk_id)
{
    struct mark *mk, *p;
    struct order *ord;
    int insert;

    ord = &kmem.orders[order - MIN_ORD];
    mk = get_mark(order, blk_id >> 5);

    // whether we need to insert it into the list
    insert = (mk->bitmap == 0);

    // clear the bit map
    if (available(mk->bitmap, blk_id))
    {
        panic("double free\n");
    }

    mk->bitmap |= (1 << (blk_id & 0x1F));

    // just insert it to the head, no need to keep the list ordered
    if (insert)
    {
        blk_id >>= 5;
        mk->lnks = LNKS(NIL, ord->head);

        // fix the pre pointer of the next mark
        if (ord->head != NIL)
        {
            p = get_mark(order, ord->head);
            p->lnks = LNKS(blk_id, NEXT_LNK(p->lnks));
        }

        ord->head = blk_id;
    }
}

// get a block
static void *get_blk(int order)
{
    struct mark *mk;
    int blk_id;
    int i;
    struct order *ord;

    ord = &kmem.orders[order - MIN_ORD];
    mk = get_mark(order, ord->head);

    if (mk->bitmap == 0)
    {
        panic("empty mark in the list\n");
    }

    for (i = 0; i < 32; i++)
    {
        if (mk->bitmap & (1 << i))
        {
            blk_id = ord->head * 32 + i;
            unmark_blk(order, blk_id);

            return blkid2mem(order, blk_id);
        }
    }

    return NULL;
}

/*when kmem.initialized is false, __internal_kfree can still be called because
  it its also used to initialize the buddy*/
static void __internal_kfree(void *mem, int order)
{
    int blk_id, buddy_id;
    struct mark *mk;
    blk_id = mem2blkid(order, mem);
    mk = get_mark(order, blk_id >> 5);

    if (available(mk->bitmap, blk_id))
    {
        panic("kmfree: double free");
    }

    buddy_id = blk_id ^ 0x0001; // blk_id and buddy_id differs in the last bit
                                // buddy must be in the same bit map
    if (!available(mk->bitmap, buddy_id) || (order == MAX_ORD))
    {
        mark_blk(order, blk_id);
    }
    else
    {
        // our buddy is also free, merge it
        unmark_blk(order, buddy_id);
        __internal_kfree(blkid2mem(order, blk_id & ~0x0001), order + 1);
    }
}

static void *__internal_kmalloc(int order)
{
    if (!kmem.initialized)
    {
        panic("__internal_kmalloc:buddy-malloc before buddy init.");
    }

    struct order *ord;
    uint8_t *up;

    ord = &kmem.orders[order - MIN_ORD];
    up = NULL;

    if (ord->head != NIL)
    {
        up = get_blk(order);
    }
    else if (order < MAX_ORD)
    {
        // if currently no block available, try to split a parent
        up = __internal_kmalloc(order + 1);

        if (up != NULL)
        {
            __internal_kfree(up + (1 << order), order);
        }
    }

    return up;
}

// allocate memory that has the size of (1 << order)
void *buddy_kmalloc(int order)
{
    uint8_t *up;

    if ((order > MAX_ORD) || (order < MIN_ORD))
    {
        panic("kmalloc: order out of range\n");
    }

    acquire(&kmem.lock);
    up = __internal_kmalloc(order);
    release(&kmem.lock);

    return up;
}

// free kernel memory, we require order parameter here to avoid
// storing size info somewhere which might break the alignment
void buddy_kmfree(void *mem, int order)
{
    if ((order > MAX_ORD) || (order < MIN_ORD) || (uint)mem & ((1 << order) - 1))
    {
        panic("kmfree: order out of range or memory unaligned\n");
    }

    acquire(&kmem.lock);
    __internal_kfree(mem, order);
    release(&kmem.lock);
}

// free a page
void buddy_free_page(char *v)
{
    buddy_kmfree(v, PGSIZE_ORDER);
}

// allocate a page
char *buddy_alloc_page(void)
{
    return buddy_kmalloc(PGSIZE_ORDER);
}

//Implement these to alloc with size,free without size
void *buddy_kmalloc2(uint32_t size)
{
    int order = buddy_get_order(size + sizeof(mblock_t) + sizeof(char));
    mblock_t *blk = (mblock_t *)kmalloc(order);
    blk->order = order;
    return (void *)blk->blocks;
}

void buddy_kmfree2(void *v)
{
    char *pblock = (char *)v;
    mblock_t *blk = (mblock_t *)container_of(pblock, mblock_t, blocks);
    int order = blk->order;
    buddy_kmfree(blk, order);
}

int buddy_get_order(uint32_t v)
{
    v = roundup_powerof2(v);

    uint32_t ord = 0;
    for (ord = 0; ord < 32; ord++)
    {
        if (v & (1 << ord))
        {
            break;
        }
    }

    if (ord < MIN_ORD)
    {
        ord = MIN_ORD;
    }
    else if (ord > MAX_ORD)
    {
        panic("buddy_get_order: order too big!");
    }

    return ord;
}
