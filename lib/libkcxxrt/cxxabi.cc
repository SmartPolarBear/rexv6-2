#include <new>
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

extern "C" void __register_frame(u8 *);

extern "C" void panic(const char *) __attribute__((noreturn)); //console.c
extern "C" struct proc *myproc(void); //proc.c
extern bool malloc_proc; //cstdfunc.cc
extern spinlock cfuncs_lock;

const std::nothrow_t std::nothrow;

void __cxa_pure_virtual(void)
{
    panic("__cxa_pure_virtual");
}

int __cxa_guard_acquire(s64 *guard)
{
    volatile u8 *x = (u8 *)guard;
    volatile u32 *l = (u32 *)(x + 4);

    pushcli();
    while (xchgl(l, 1) != 0)
        ; /* spin */

    if (*x)
    {
        xchgl(l, 0);
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
    xchgl(l, 0);
    popcli();
}

void __cxa_guard_abort(s64 *guard)
{
    volatile u8 *x = (u8 *)guard;
    volatile u32 *l = (u32 *)(x + 4);

    xchgl(l, 0);
    popcli();
}

int __cxa_atexit(void (*f)(void *), void *p, void *d)
{
    return 0;
}

spinlock cxx_terminate_lock;

static void
cxx_terminate(void)
{
    // static std::atomic_flag recursive = ATOMIC_FLAG_INIT;
    static bool recursive = false;

    // In GCC, we can actually rethrow and catch the exception that led
    // to the terminate.  However, terminate may be called for other
    // reasons, such as a "throw" without an active exception, so if we
    // don't have an active exception, this will call us recursively.
    try
    {
        acquire(&cxx_terminate_lock);
        // if (!recursive.test_and_set())
        //     throw;
        if (!recursive)
        {
            recursive = true;
            throw;
        }
        else
        {
            recursive = true;
        }

        release(&cxx_terminate_lock);
    }
    catch (const std::exception &e)
    {
        release(&cxx_terminate_lock);
        panic(e.what());
    }
    catch (...)
    {
        release(&cxx_terminate_lock);
        panic("unhandled exception");
    }

    panic("cxx terminate");
}

static void
cxx_unexpected(void)
{
    panic("cxx unexpected");
}

void *__dso_handle;

namespace __cxxabiv1
{
void (*__terminate_handler)() = cxx_terminate;
void (*__unexpected_handler)() = cxx_unexpected;
}; // namespace __cxxabiv1

extern "C" int dl_iterate_phdr(void);
int dl_iterate_phdr(void)
{
    return -1;
}

extern "C" void __stack_chk_fail(void);
void __stack_chk_fail(void)
{
    panic("stack_chk_fail");
}

extern "C" void *__cxa_get_globals(void);
void *
__cxa_get_globals(void)
{
    return myproc()->__cxa_eh_global;
}

extern "C" void *__cxa_get_globals_fast(void);
void *
__cxa_get_globals_fast(void)
{
    return myproc()->__cxa_eh_global;
}

void initcpprt(void)
{
    constexpr auto MAGIC = 5;

    initlock(&cxx_terminate_lock, "cxx_terminate_lock");
    initlock(&cfuncs_lock, "cfuncs");

    extern u8 __EH_FRAME_BEGIN__[];
    __register_frame(__EH_FRAME_BEGIN__);

    // Initialize lazy exception handling data structures
    try
    {
        throw MAGIC;
    }
    catch (int &x)
    {
        if (x != MAGIC)
        {
            panic("initcpprt:x != MAGIC");
        }
        malloc_proc = true;
        return;
    }

    panic("no catch");
}