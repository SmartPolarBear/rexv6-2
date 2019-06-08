/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-08 20:17:10
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/x86.h"
#include "xv6/proc.h"
#include "xv6/spinlock.h"
#include "xv6/rand.h"

struct
{
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void pinit(void)
{
    initlock(&ptable.lock, "ptable");
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc *allocproc(void)
{
    struct proc *p;
    char *sp;

    acquire(&ptable.lock);

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->state == UNUSED)
        {
            goto found;
        }
    }

    release(&ptable.lock);
    return 0;

found:
    p->state = EMBRYO;
    p->pid = nextpid++;
    p->ustack = 0;
    p->mthread = 1;
    p->tickets = DEFAULT_TICKETS;

    release(&ptable.lock);

    // Allocate kernel stack.
    if ((p->kstack = kalloc()) == 0)
    {
        p->state = UNUSED;
        return 0;
    }
    sp = p->kstack + KSTACKSIZE;

    // Leave room for trap frame.
    sp -= sizeof *p->tf;
    p->tf = (struct trapframe *)sp;

    // Set up new context to start executing at forkret,
    // which returns to trapret.
    sp -= 4;
    *(uint *)sp = (uint)trapret;

    sp -= sizeof *p->context;
    p->context = (struct context *)sp;
    memset(p->context, 0, sizeof *p->context);
    p->context->eip = (uint)forkret;

    for (int i = SIGNAL_MIN; i < SIGNAL_COUNT; i++)
    {
        p->sighandlers[i] = (sighandler_t)-1;
    }
    p->cstack.head = 0;
    for (cstackframe_t *sig = p->cstack.frames; sig <= &p->cstack.frames[SIGNAL_COUNT - 1]; sig++)
    {
        sig->used = FALSE;
    }

    p->ignore_signals = FALSE;
    p->sigpause_involked = FALSE;

    p->starttime = ticks;

    return p;
}

//PAGEBREAK: 32
// Set up first user process.
void userinit(void)
{
    struct proc *p;
    extern char _binary_initcode_start[], _binary_initcode_size[];

    p = allocproc();

    initproc = p;
    if ((p->pgdir = setupkvm()) == 0)
        panic("userinit: out of memory?");
    inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
    p->sz = PGSIZE;
    memset(p->tf, 0, sizeof(*p->tf));
    p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
    p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
    p->tf->es = p->tf->ds;
    p->tf->ss = p->tf->ds;
    p->tf->eflags = FL_IF;
    p->tf->esp = PGSIZE;
    p->tf->eip = 0; // beginning of initcode.S

    safestrcpy(p->name, "initcode", sizeof(p->name));
    p->cwd = namei("/");

    // this assignment to p->state lets other cores
    // run this process. the acquire forces the above
    // writes to be visible, and the lock is also needed
    // because the assignment might not be atomic.
    acquire(&ptable.lock);

    p->state = RUNNABLE;

    release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int growproc(int n)
{
    uint sz;

    sz = proc->sz;
    if (n > 0)
    {
        if ((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
            return -1;
    }
    else if (n < 0)
    {
        if ((sz = deallocuvm(proc->pgdir, sz, sz + n)) == 0)
            return -1;
    }
    proc->sz = sz;
    switchuvm(proc);
    return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int fork(void)
{
    int i, pid;
    struct proc *np;

    // Allocate process.
    if ((np = allocproc()) == 0)
    {
        return -1;
    }

    // Copy process state from p.
    if ((np->pgdir = copyuvm(proc->pgdir, proc->sz)) == 0)
    {
        kfree(np->kstack);
        np->kstack = 0;
        np->state = UNUSED;
        return -1;
    }
    np->sz = proc->sz;
    np->parent = proc;
    np->ustack = proc->ustack;
    *np->tf = *proc->tf;

    // Clear %eax so that fork returns 0 in the child.
    np->tf->eax = 0;

    for (i = 0; i < NOFILE; i++)
        if (proc->ofile[i])
            np->ofile[i] = filedup(proc->ofile[i]);
    np->cwd = idup(proc->cwd);

    for (int i = SIGNAL_MIN; i < SIGNAL_COUNT; i++)
    {
        np->sighandlers[i] = proc->sighandlers[i];
    }

    safestrcpy(np->name, proc->name, sizeof(proc->name));

    pid = np->pid;

    acquire(&ptable.lock);

    np->state = RUNNABLE;

    release(&ptable.lock);

    return pid;
}

// Create a new thread.
int thread_create(void)
{
    int i, pid;
    struct proc *np;

    // Allocate process.
    if ((np = allocproc()) == 0)
    {
        return -1;
    }

    // Copy process state from p.
    if ((np->pgdir = copystackuvm(proc->pgdir, proc->sz, proc->ustack)) == 0)
    {
        kfree(np->kstack);
        np->kstack = 0;
        np->state = UNUSED;
        return -1;
    }
    np->sz = proc->sz;
    if (proc->mthread)
    {
        np->parent = proc;
    }
    else
    {
        np->parent = proc->parent;
    }
    np->ustack = proc->ustack;
    np->mthread = 0;
    *np->tf = *proc->tf;

    // Clear %eax so that fork returns 0 in the child.
    np->tf->eax = 0;

    for (i = 0; i < NOFILE; i++)
        if (proc->ofile[i])
            np->ofile[i] = filedup(proc->ofile[i]);
    np->cwd = idup(proc->cwd);

    safestrcpy(np->name, proc->name, sizeof(proc->name));

    pid = np->pid;

    acquire(&ptable.lock);

    np->state = RUNNABLE;

    release(&ptable.lock);

    return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void exit(void)
{
    // Only main thread can call this.
    if (!proc->mthread)
        panic("exit: mthread type error");

    struct proc *p;
    int fd;

    if (proc == initproc)
        panic("init exiting");

    // Close all open files.
    for (fd = 0; fd < NOFILE; fd++)
    {
        if (proc->ofile[fd])
        {
            fileclose(proc->ofile[fd]);
            proc->ofile[fd] = 0;
        }
    }

    begin_op();
    iput(proc->cwd);
    end_op();
    proc->cwd = 0;

    acquire(&ptable.lock);

    // Exit all the threads the process has.
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if ((p->parent == proc) && (!p->mthread))
            p->state = ZOMBIE;
    }

    // Parent might be sleeping in wait().
    wakeup1(proc->parent);

    // Pass abandoned children to init.
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->parent == proc)
        {
            p->parent = initproc;
            if (p->state == ZOMBIE)
                wakeup1(initproc);
        }
    }

    // Jump into the scheduler, never to return.
    proc->state = ZOMBIE;
    sched();
    panic("zombie exit");
}

// Exit the current thread.
void thread_exit(void)
{
    // Only non-main thread can call this.
    if (proc->mthread)
        panic("thread_exit: mthread type error");

    acquire(&ptable.lock);

    // Parent might be sleeping in wait().
    wakeup1(proc->parent);

    // Jump into the scheduler, never to return.
    proc->state = ZOMBIE;
    sched();
    panic("zombie exit");
}

// Wait for a child process (main thread) to exit and return its pid.
// Return -1 if this process has no children (main thread).
int wait(void)
{
    struct proc *p;
    int havekids, pid;

    acquire(&ptable.lock);
    for (;;)
    {
        // Scan through table looking for exited children (main thread).
        havekids = 0;
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        {
            if ((!p->mthread) || (p->parent != proc))
                continue;
            havekids = 1;
            if (p->state == ZOMBIE)
            {
                // Found one.
                pid = p->pid;
                kfree(p->kstack);
                p->kstack = 0;
                freevm(p->pgdir);
                p->mthread = 1;
                p->ustack = 0;
                p->pid = 0;
                p->parent = 0;
                p->name[0] = 0;
                p->killed = 0;
                p->state = UNUSED;
                release(&ptable.lock);
                return pid;
            }
        }

        // No point waiting if we don't have any children (main thread).
        if (!havekids || proc->killed)
        {
            release(&ptable.lock);
            return -1;
        }

        // Wait for children to exit.  (See wakeup1 call in proc_exit.)
        sleep(proc, &ptable.lock); //DOC: wait-sleep
    }
}

// Wait for a child process (non-main thread) to exit and return its pid.
// Return -1 if this process has no children (non-main thread).
int thread_wait(void)
{
    struct proc *p;
    int havekids, pid;

    acquire(&ptable.lock);
    for (;;)
    {
        // Scan through table looking for exited children (non-main thread).
        havekids = 0;
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        {
            if ((p->mthread) || (p->parent != proc))
                continue;
            havekids = 1;
            if (p->state == ZOMBIE)
            {
                // Found one.
                pid = p->pid;
                kfree(p->kstack);
                p->kstack = 0;
                freestackvm(p->pgdir, p->ustack);
                p->mthread = 1;
                p->ustack = 0;
                p->pid = 0;
                p->parent = 0;
                p->name[0] = 0;
                p->killed = 0;
                p->state = UNUSED;
                release(&ptable.lock);
                return pid;
            }
        }

        // No point waiting if we don't have any children (non-main thread).
        if (!havekids || proc->killed)
        {
            release(&ptable.lock);
            return -1;
        }

        // Wait for children to exit.  (See wakeup1 call in proc_exit.)
        sleep(proc, &ptable.lock); //DOC: wait-sleep
    }
}

int lottery_total(void)
{
    struct proc *p;
    int ticket_aggregate = 0;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->state == RUNNABLE)
        {
            ticket_aggregate += p->tickets;
        }
    }

    return ticket_aggregate;
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void scheduler(void)
{
    struct proc *p;

    int count = 0;
    long golden_ticket = 0;
    int total_no_tickets = 0;

    for (;;)
    {
        // Enable interrupts on this processor.
        sti();

        // Loop over process table looking for process to run.
        acquire(&ptable.lock);

        //resetting lottery scheduling variables.
        golden_ticket = 0;
        count = 0;
        total_no_tickets = 0;

        //calculate total tickets
        total_no_tickets = lottery_total();

        //Generate a ticket for all
        golden_ticket = random_at_most(total_no_tickets);

        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        {
            if (p->state != RUNNABLE)
                continue;

            if ((count + p->tickets) < golden_ticket)
            {
                count += p->tickets;
                continue;
            }

            // Switch to chosen process.  It is the process's job
            // to release ptable.lock and then reacquire it
            // before jumping back to us.
            proc = p;
            switchuvm(p);
            p->state = RUNNING;
            swtch(&cpu->scheduler, p->context);
            switchkvm();

            // Process is done running for now.
            // It should have changed its p->state before coming back.
            proc = 0;
            break;
        }
        release(&ptable.lock);
    }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void)
{
    int intena;

    if (!holding(&ptable.lock))
        panic("sched ptable.lock");
    if (cpu->ncli != 1)
        panic("sched locks");
    if (proc->state == RUNNING)
        panic("sched running");
    if (readeflags() & FL_IF)
        panic("sched interruptible");
    intena = cpu->intena;
    swtch(&proc->context, cpu->scheduler);
    cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void yield(void)
{
    acquire(&ptable.lock); //DOC: yieldlock
    proc->state = RUNNABLE;
    sched();
    release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void forkret(void)
{
    static int first = 1;
    // Still holding ptable.lock from scheduler.
    release(&ptable.lock);

    if (first)
    {
        // Some initialization functions must be run in the context
        // of a regular process (e.g., they call sleep), and thus cannot
        // be run from main().
        first = 0;
        iinit(ROOTDEV);
        initlog(ROOTDEV);
    }

    // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void sleep(void *chan, struct spinlock *lk)
{
    if (proc == 0)
        panic("sleep");

    if (lk == 0)
        panic("sleep without lk");

    // Must acquire ptable.lock in order to
    // change p->state and then call sched.
    // Once we hold ptable.lock, we can be
    // guaranteed that we won't miss any wakeup
    // (wakeup runs with ptable.lock locked),
    // so it's okay to release lk.
    if (lk != &ptable.lock)
    {                          //DOC: sleeplock0
        acquire(&ptable.lock); //DOC: sleeplock1
        release(lk);
    }

    // Go to sleep.
    proc->chan = chan;
    proc->state = SLEEPING;
    sched();

    // Tidy up.
    proc->chan = 0;

    // Reacquire original lock.
    if (lk != &ptable.lock)
    { //DOC: sleeplock2
        release(&ptable.lock);
        acquire(lk);
    }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
    struct proc *p;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        if (p->state == SLEEPING && p->chan == chan)
            p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void wakeup(void *chan)
{
    acquire(&ptable.lock);
    wakeup1(chan);
    release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int kill(int pid)
{
    struct proc *p;

    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->pid == pid)
        {
            p->killed = 1;
            // Wake process from sleep if necessary.
            if (p->state == SLEEPING)
                p->state = RUNNABLE;
            release(&ptable.lock);
            return 0;
        }
    }
    release(&ptable.lock);
    return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void procdump(void)
{

    static char *states[] = {
        [UNUSED] "unused",
        [EMBRYO] "embryo",
        [SLEEPING] "sleep ",
        [RUNNABLE] "runble",
        [RUNNING] "run   ",
        [ZOMBIE] "zombie"};
    int i;
    struct proc *p;
    char *state;
    uint pc[10];

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->state == UNUSED)
            continue;
        if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
            state = states[p->state];
        else
            state = "???";
        cprintf("%d %s %s %d", p->pid, state, p->name, p->tickets);
        if (p->state == SLEEPING)
        {
            getcallerpcs((uint *)p->context->ebp + 2, pc);
            for (i = 0; i < 10 && pc[i] != 0; i++)
                cprintf(" %p", pc[i]);
        }
        cprintf("\n");
    }
}

int lastproc_pid(void)
{
    int pid = -1, pptr = 0, maxtime = 0;

    struct node
    {
        int pid, time;
    } pids[NPROC];
    memset(pids, 0, sizeof(pids));

    acquire(&ptable.lock);

    for (struct proc *p = ptable.proc;
         p < &ptable.proc[NPROC];
         p++)
    {
        if (p->state == RUNNING)
        {
            pid = p->pid;
            break;
        }
        else if (p->state == SLEEPING)
        {
            pids[pptr++] = (struct node){p->pid, p->starttime};
            maxtime = MAX(p->starttime, maxtime);
        }
    }

    release(&ptable.lock);

    if (pid < 0 && pptr <= MIN_PROC_NUM)
        return -1;

    if (pid < 0)
    {
        for (int i = 0; i < pptr; i++)
        {
            if (pids[i].time == maxtime)
            {
                pid = pids[i].pid;
                break;
            }
        }
    }

    if (pid < 0)
        return -1;

    return pid;
}

int cstk_push(cstack_t *cstack, int dest, int signum)
{
    struct proc *p = NULL;
    cstackframe_t *newsig = NULL;
    for (newsig = cstack->frames; newsig < &cstack->frames[SIGNAL_MAX - SIGNAL_MIN]; newsig++)
    {
        if (cas(&newsig->used, FALSE, TRUE))
            goto found;
    }
    return 0;
found:
    newsig->dest = dest;
    newsig->signum = signum;
    do
    {
        newsig->next = cstack->head;
    } while (!cas((int *)&cstack->head, (int)newsig->next, (int)newsig));

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->pid == dest)
        {
            while (p->state == NEG_SLEEPING)
            {
                // busy-wait
            }
            if (cas(&p->sigpause_involked, 1, 0)) // only one thread will change the state to RUNNABLE
                p->state = RUNNABLE;

            break;
        }
    }

    return 1;
}

cstackframe_t *cstk_pop(cstack_t *cstack)
{
    cstackframe_t *top = NULL;
    do
    {
        top = cstack->head;
        if (top == 0)
            break;
    } while (!cas((int *)&cstack->head, (int)top, (int)top->next));
    return top;
}

sighandler_t sigset(int signum, sighandler_t sighandler)
{
    sighandler_t previous = proc->sighandlers[signum];
    proc->sighandlers[signum] = sighandler;
    return previous;
}

int sigsend(int pid, int signum)
{
    struct proc *p = NULL;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
        if (p->pid == pid)
            goto found;
    }
    return -1; // pid wan't found, meaning it's not a valid pid. return error
found:
    if (cstk_push(&p->cstack, pid, signum))
    {
        return 0; //success
    }

    return -1; //cstack full
}

void sigret(void)
{
    memmove(proc->tf, &proc->oldtf, sizeof(struct trapframe));
    proc->ignore_signals = FALSE;
}

void sigpause(void)
{
    pushcli();
    if (!cas(&proc->state, RUNNING, NEG_SLEEPING))
        panic("sigpause: cas #1 failed");

    if (proc->cstack.head == 0)
    {
        proc->chan = 0;
        proc->sigpause_involked = TRUE;
        sched();
    }
    else
    {
        if (!cas(&proc->state, NEG_SLEEPING, RUNNING))
            panic("sigpause: cas #2 failed");
    }
    popcli();
}

void handle_signals(struct trapframe *tf)
{
    if (proc == 0)
        return; // no proc is defined for this CPU

    if (proc->ignore_signals)
        return; // currently handling a signal

    if ((tf->cs & 3) != DPL_USER)
        return; // CPU isn't at privilege level 3, hence in user mode

    struct cstackframe *top = cstk_pop(&proc->cstack);
    if (top == (struct cstackframe *)0)
        return; // no pending signals

    if (proc->sighandlers[top->signum] == (sighandler_t)-1)
        return; // default signal handler, ignore the signal

    proc->ignore_signals = TRUE;

    memmove(&proc->oldtf, proc->tf, sizeof(struct trapframe)); //backing up trap frame
    proc->tf->esp -= (uint)&invoke_sigret_end - (uint)&invoke_sigret_start;
    memmove((void *)proc->tf->esp, invoke_sigret_start, (uint)&invoke_sigret_end - (uint)&invoke_sigret_start);

    *((int *)(proc->tf->esp - 4)) = top->signum;
    *((int *)(proc->tf->esp - 8)) = proc->tf->esp; // sigret system call code address
    proc->tf->esp -= 8;

    proc->tf->eip = (uint)proc->sighandlers[top->signum]; // trapret will resume into signal handler
    top->used = 0;                                        // free the cstackframe
}
