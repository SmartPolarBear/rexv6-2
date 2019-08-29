
#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/mmu.h"
#include "arch/x86/x86.h"
#include "xv6/proc.h"
#include "xv6/spinlock.h"

extern "C" void abort(void);
extern "C" void *malloc(size_t);
extern "C" void free(void *);

bool malloc_proc = false;

void abort(void)
{
    panic("abort");
}

void *
malloc(size_t n)
{
    if (malloc_proc)
    {
        if (n > sizeof(myproc()->exception_buf))
        {
            panic("kcxxrt malloc out of sizeof(myproc()->exception_buf)");
        }
        if (!cas(&myproc()->exception_inuse, false, true))
        {
            panic("kcxxrt can't do cmpxch(&myproc()->exception_inuse, 0, 1)");
        }
        return myproc()->exception_buf;
    }

    return kmalloc(n);
}

void free(void *vp)
{
    if (vp == myproc()->exception_buf)
    {
        xchg_int(&myproc()->exception_inuse, false);
        return;
    }

    kmfree(vp);
}
