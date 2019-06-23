#include "xv6/types.h"
#include "xv6/x86.h"
#include "xv6/defs.h"
#include "xv6/date.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/proc.h"

int sys_fork(void)
{
    return fork();
}

int sys_exit(void)
{
    exit();
    return 0; // not reached
}

int sys_wait(void)
{
    return wait();
}

int sys_kill(void)
{
    int pid;

    if (argint(0, &pid) < 0)
        return -1;
    return kill(pid);
}

int sys_getpid(void)
{
    return proc->pid;
}

int sys_sbrk(void)
{
    int addr;
    int n;

    if (argint(0, &n) < 0)
        return -1;
    addr = proc->sz;
    if (growproc(n) < 0)
        return -1;
    return addr;
}

int sys_sleep(void)
{
    int n;
    uint ticks0;

    if (argint(0, &n) < 0)
        return -1;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n)
    {
        if (proc->killed)
        {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}

int sys_settickets(void)
{
    int ticket_num = 0;
    if (argint(0, &ticket_num) < 0)
    {
        proc->tickets = DEFAULT_TICKETS;
    }
    else
    {
        proc->tickets = ticket_num;
    }
    return 0;
}

int sys_sigset(void)
{
    int signum, haddr;
    if (argint(0, &signum) < 0)
        return -1;
    if (argint(1, &haddr) < 0)
        return -1;
    return (int)sigset(signum, (sighandler_t)haddr);
}

int sys_sigsend(void)
{
    int pid, signum;
    if (argint(0, &pid) < 0)
        return -1;
    if (argint(1, &signum) < 0)
        return -1;

    return sigsend(pid, signum);
}

int sys_sigpause(void)
{
    sigpause();
    return 1;
}

int sys_sigret(void)
{
    sigret();
    return 1;
}

int sys_clone(void)
{
    int func, arg, stack;
    //clone(void *(*func) (void*), void *arg, void *stack)
    if (argint(0, &func) < 0)
        return -1;
    if (argint(1, &arg) < 0)
        return -1;
    if (argint(2, &stack) < 0)
        return -1;

    return clone(func, (void *)arg, (void *)stack);
}

int sys_join(void)
{
    int pid, stack, retval;
    if (argint(0, &pid) < 0)
        return -1;
    if (argint(1, &stack) < 0)
        return -1;
    if (argint(2, &retval) < 0)
        return -1;

    return join(pid, (void **)stack, (void **)retval);
}

int sys_texit(void)
{
    int retval;
    if (argint(0, &retval) < 0)
        return -1;
    texit((void *)retval);
    return 0;
}

int sys_mutex_lock()
{
    int mutex_id;
    if (argint(0, &mutex_id) < 0)
        return -1;
    if (mutex_id < 0 || mutex_id > 31)
        return -2;
    if (proc->mtable_shared[mutex_id].isfree)
        return -3;

    mutex_lock(mutex_id);
    return 0;
}

int sys_mutex_unlock()
{
    int mutex_id;
    if (argint(0, &mutex_id) < 0)
        return -1;
    if (mutex_id < 0 || mutex_id > 31)
        return -2;
    if (proc->mtable_shared[mutex_id].isfree)
        return -3;

    mutex_unlock(mutex_id);
    return 0;
}

int sys_mutex_init()
{
    int i;
    acquire(proc->mlock_shared);
    for (i = 0; i < 32; i++)
    {
        if (proc->mtable_shared[i].isfree)
        {
            proc->mtable_shared[i].isfree = 0;
            proc->mtable_shared[i].flag = 0;
            proc->mtable_shared[i].cond = proc;
            initlock(&proc->mtable_shared[i].lock, "mutex");
            release(proc->mlock_shared);
            return i;
        }
    }
    release(proc->mlock_shared);
    return -1;
}

int sys_mutex_destroy()
{
    int mutex_id;
    if (argint(0, &mutex_id) < 0)
        return -1;
    if (mutex_id < 0 || mutex_id > 31)
        return -2;
    if (proc->mtable_shared[mutex_id].isfree)
        return -3;

    acquire(proc->mlock_shared);
    proc->mtable_shared[mutex_id].isfree = 1;
    release(proc->mlock_shared);
    return 0;
}
