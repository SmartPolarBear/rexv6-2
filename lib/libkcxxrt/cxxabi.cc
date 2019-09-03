#include "xv6/spinlock.h"
#include "xv6/param.h"
#include "xv6/mmu.h"
#include "xv6/proc.h"
#include "arch/x86/x86.h"

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using s8 = char;
using s16 = short;
using s32 = int;
using s64 = long long;

extern "C" //c symbols for cxxrt
{
    void cprintf(char *, ...);                          //console.c
    void panic(const char *) __attribute__((noreturn)); //console.c

    int dl_iterate_phdr(void);
    void __stack_chk_fail_local(void);
    void __stack_chk_fail(void);
}

void __cxa_pure_virtual(void)
{
    panic("__cxa_pure_virtual");
}

int __cxa_guard_acquire(s64 *guard)
{
    volatile u8 *x = (u8 *)guard;
    volatile u32 *l = (u32 *)(x + 4);

    pushcli();
    while (xchg(l, 1) != 0)
        ; /* spin */

    if (*x)
    {
        xchg(l, 0);
        popcli();
        return 0;
    }
    return 1;
}

void __cxa_guard_release(s64 *guard)
{
    volatile u8 *x = (u8 *)guard;
    volatile u32 *l = (u32 *)(x + 4);

    *x = 1;
    __sync_synchronize();
    xchg(l, 0);
    popcli();
}

void __cxa_guard_abort(s64 *guard)
{
    volatile u8 *x = (u8 *)guard;
    volatile u32 *l = (u32 *)(x + 4);

    xchg(l, 0);
    popcli();
}

int __cxa_atexit(void (*f)(void *), void *p, void *d)
{
    return 0;
}

static void cxx_terminate(void)
{
    panic("cxx terminate");
}

static void cxx_unexpected(void)
{
    panic("cxx unexpected");
}

void *__dso_handle;

namespace __cxxabiv1
{
void (*__terminate_handler)() = cxx_terminate;
void (*__unexpected_handler)() = cxx_unexpected;
}; // namespace __cxxabiv1

int dl_iterate_phdr(void)
{
    return -1;
}

void __stack_chk_fail_local(void)
{
    panic("__stack_chk_fail_local");
}

void __stack_chk_fail(void)
{
    panic("stack_chk_fail");
}

extern "C" 
void initcpprt(void)
{
    using ctor_func = void (*)();
    extern ctor_func sinit_array[];             // zero terminated constructors table
    extern ctor_func spercpuinit_array_start[]; // per-cpu zero terminated constructors table

    for (size_t i = 0; sinit_array[i]; i++)
    {
        sinit_array[i]();
    }

    for (size_t i = 0; spercpuinit_array_start[i]; i++)
    {
        spercpuinit_array_start[i]();
    }
}
