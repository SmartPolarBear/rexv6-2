/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-23 17:45:09
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/proc.h"
#include "xv6/x86.h"
#include "xv6/syscall.h"

#define IN_FIRST_PAGE(addr) (proc->pid > 1 && (addr) < PGSIZE)
#define IN_PROC_STACK(addr) ((addr) >= STACKBASE - proc->stk_sz * PGSIZE && (addr) <= STACKBASE)

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int fetchint(uint addr, int *ip)
{
    // if (addr >= proc->sz || addr + 4 > proc->sz)
    if (addr >= STACKBASE || addr + 4 > STACKBASE)
    {
        return ERROR_OUT_OF_STACK;
    }

    if (IN_FIRST_PAGE(addr))
    {
        return ERROR_IN_FIRST_PAGE;
    }

    *ip = *(int *)(addr);
    return ERROR_SUCCESS;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uint addr, char **pp)
{
    char *s, *ep;

    if (addr >= proc->sz && !IN_PROC_STACK(addr))
    {
        return ERROR_OUT_OF_STACK;
    }

    if (IN_FIRST_PAGE(addr))
    {
        return ERROR_IN_FIRST_PAGE;
    }

    *pp = (char *)addr;
    ep = (char *)STACKBASE;
    for (s = *pp; s < ep; s++)
        if (*s == 0)
            return s - *pp;

    return ERROR_INVALID_VAL;
}

// Fetch the nth 32-bit system call argument.
int argint(int n, int *ip)
{
    return fetchint(proc->tf->esp + 4 + 4 * n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int argptr(int n, char **pp, int size)
{
    int i, hret = 0;

    if ((hret = argint(n, &i)) < 0)
    {
        return hret;
    }

    // if (size < 0 || (uint)i >= proc->sz || (uint)i + size > proc->sz)
    if (size < 0 || (uint)i >= STACKBASE || (uint)i + size > STACKBASE)
    {
        return ERROR_INVALID_VAL;
    }

    *pp = (char *)i;
    return ERROR_SUCCESS;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp)
{
    int addr, hret = 0;
    if ((hret = argint(n, &addr)) < 0)
    {
        return hret;
    }
    return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_mount(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_unmount(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_getcrtc(void);
extern int sys_setcrtc(void);
extern int sys_getcurpos(void);
extern int sys_setcurpos(void);
extern int sys_playsound(void);
extern int sys_nosound(void);
extern int sys_geteditstatus(void);
extern int sys_seteditstatus(void);
extern int sys_gettime(void);

extern int sys_lock_create(void);
extern int sys_lock_acquire(void);
extern int sys_lock_release(void);
extern int sys_lock_holding(void);
extern int sys_lock_free(void);

extern int sys_semaphore_create(void);
extern int sys_semaphore_acquire(void);
extern int sys_semaphore_release(void);
extern int sys_semaphore_getcounter(void);
extern int sys_semaphore_free(void);

extern int sys_rwlock_create(void);
extern int sys_rwlock_acquire_read(void);
extern int sys_rwlock_release_read(void);
extern int sys_rwlock_holding_read(void);
extern int sys_rwlock_acquire_write(void);
extern int sys_rwlock_release_write(void);
extern int sys_rwlock_holding_write(void);
extern int sys_rwlock_free(void);

extern int sys_settickets(void);

extern int sys_sigset(void);
extern int sys_sigsend(void);
extern int sys_sigret(void);
extern int sys_sigpause(void);

//threads
extern int sys_clone(void);
extern int sys_join(void);
extern int sys_texit(void);
extern int sys_mutex_init(void);
extern int sys_mutex_destroy(void);
extern int sys_mutex_lock(void);
extern int sys_mutex_unlock(void);

static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,
    [SYS_exit] sys_exit,
    [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,
    [SYS_read] sys_read,
    [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,
    [SYS_fstat] sys_fstat,
    [SYS_chdir] sys_chdir,
    [SYS_dup] sys_dup,
    [SYS_getpid] sys_getpid,
    [SYS_sbrk] sys_sbrk,
    [SYS_sleep] sys_sleep,
    [SYS_uptime] sys_uptime,
    [SYS_open] sys_open,
    [SYS_write] sys_write,
    [SYS_mknod] sys_mknod,
    [SYS_unlink] sys_unlink,
    [SYS_link] sys_link,
    [SYS_mkdir] sys_mkdir,
    [SYS_close] sys_close,
    [SYS_getcrtc] sys_getcrtc,
    [SYS_setcrtc] sys_setcrtc,
    [SYS_getcurpos] sys_getcurpos,
    [SYS_setcurpos] sys_setcurpos,
    [SYS_geteditstatus] sys_geteditstatus,
    [SYS_seteditstatus] sys_seteditstatus,
    [SYS_mount] sys_mount,
    [SYS_unmount] sys_unmount,
    [SYS_playsound] sys_playsound,
    [SYS_nosound] sys_nosound,
    [SYS_gettime] sys_gettime,
    [SYS_lock_create] sys_lock_create,
    [SYS_lock_acquire] sys_lock_acquire,
    [SYS_lock_release] sys_lock_release,
    [SYS_lock_holding] sys_lock_holding,
    [SYS_lock_free] sys_lock_free,
    [SYS_semaphore_create] sys_semaphore_create,
    [SYS_semaphore_acquire] sys_semaphore_acquire,
    [SYS_semaphore_release] sys_semaphore_release,
    [SYS_semaphore_getcounter] sys_semaphore_getcounter,
    [SYS_semaphore_free] sys_semaphore_free,
    [SYS_rwlock_create] sys_rwlock_create,
    [SYS_rwlock_acquire_read] sys_rwlock_acquire_read,
    [SYS_rwlock_release_read] sys_rwlock_release_read,
    [SYS_rwlock_holding_read] sys_rwlock_holding_read,
    [SYS_rwlock_acquire_write] sys_rwlock_acquire_write,
    [SYS_rwlock_release_write] sys_rwlock_release_write,
    [SYS_rwlock_holding_write] sys_rwlock_holding_write,
    [SYS_rwlock_free] sys_rwlock_free,
    [SYS_settickets] sys_settickets,

    [SYS_sigset] sys_sigset,
    [SYS_sigsend] sys_sigsend,
    [SYS_sigret] sys_sigret,
    [SYS_sigpause] sys_sigpause,

    [SYS_clone] sys_clone,
    [SYS_join] sys_join,
    [SYS_texit] sys_texit,
    [SYS_mutex_init] sys_mutex_init,
    [SYS_mutex_destroy] sys_mutex_destroy,
    [SYS_mutex_lock] sys_mutex_lock,
    [SYS_mutex_unlock] sys_mutex_unlock,
};

void syscall(void)
{
    int num;
    num = proc->tf->eax;
    if (num > 0 && num < NELEM(syscalls) && syscalls[num])
    {
        proc->tf->eax = syscalls[num]();
    }
    else
    {
        cprintf("%d %s: unknown sys call %d\n",
                proc->pid, proc->name, num);
        proc->tf->eax = -1;
    }
}
