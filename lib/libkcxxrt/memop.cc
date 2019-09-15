#include "xv6/types.h"
#include "xv6/kalloc.h"
#include "xv6/defs.h"

void *operator new(size_t size)
{
    // panic("fuck");
    return kmalloc(size);
}

void *operator new[](size_t size)
{
    // panic("fuck");
    return kmalloc(size);
}

void operator delete(void *p)
{
    // panic("fuck");
    kmfree(p);
}

void operator delete[](void *p)
{
    // panic("fuck");

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
    //do nothing for palcement new
    ;
}

void operator delete[](void *, void *) noexcept
{
    //do nothing for palcement new[]
    ;
}