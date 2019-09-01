#include "xv6/types.h"
#include "xv6/kalloc.h"

void *operator new(size_t size)
{
    return kmalloc(size);
}

void *operator new[](size_t size)
{
    return kmalloc(size);
}

void operator delete(void *p)
{
    kmfree(p);
}

void operator delete[](void *p)
{
    kmfree(p);
}

//placement new/deletes
void *operator new(size_t, void *p) noexcept
{
    return p;
}

void *operator new[](size_t, void *p) noexcept
{
    return p;
}

void operator delete(void *, void *)noexcept
{
}

void operator delete[](void *, void *) noexcept
{
}