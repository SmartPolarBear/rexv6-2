#if !defined(__INCLUDE_ARCH_X86_ATOMIC_H)
#define __INCLUDE_ARCH_X86_ATOMIC_H

#include "xv6/types.h"

static inline void
atom_inc(volatile int *num)
{
    asm volatile("lock incl %0"
                 : "=m"(*num));
}

static inline void
lock_inc(uint *mem)
{
    asm volatile("lock incl %0"
                 : "=m"(mem));
}

static inline void
lock_dec(uint *mem)
{
    asm volatile("lock decl %0"
                 : "=m"(mem));
}

static inline void
lock_add(uint *mem, uint n)
{
    asm volatile("lock add %0, %1"
                 : "=m"(mem)
                 : "d"(n));
}

#endif // __INCLUDE_ARCH_X86_ATOMIC_H
