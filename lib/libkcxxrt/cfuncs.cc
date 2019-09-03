
#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"


extern "C" void abort(void);
extern "C" void *malloc(size_t);
extern "C" void free(void *);

extern "C" void abort(void)
{
    panic("kernel cxxrt abort called");
}

extern "C" void *malloc(size_t n)
{
    return kmalloc(n);
}

extern "C" void free(void *vp)
{
    kmfree(vp);
}
