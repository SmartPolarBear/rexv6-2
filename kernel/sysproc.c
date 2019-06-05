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

int sys_sigregister(void)
{
    int signum = 0;
    if (argint(0, &signum) < 0)
        return -1;

    int handler = 0, haddr = 0;
    if (argint(1, &haddr) < 0)
        return -1;
    handler = (void *)haddr;

    int trampoline = 0, trampaddr = 0;
    if (argint(2, &trampaddr) < 0)
        return -1;
    trampoline = (void *)trampaddr;
    
    if (!VALIDATE_HANDLER(handler))
        cprintf("(sysproc.c)invalid handler.\n");

    return (int)signal_register_handler(signum, handler, trampoline);
}

int sys_sigreturn(void)
{
    signal_return();

    return 0;
}