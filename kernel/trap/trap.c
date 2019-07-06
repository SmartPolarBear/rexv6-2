#include "xv6/types.h"
#include "xv6/defs.h"
#include "xv6/param.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"
#include "xv6/proc.h"
#include "xv6/x86.h"
#include "xv6/traps.h"
#include "xv6/spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[]; // in vectors.S: array of 256 entry pointers

struct spinlock tickslock;
uint ticks;
uint new_ticks;
void tvinit(void)
{
    int i;

    for (i = 0; i < 256; i++)
        SETGATE(idt[i], 0, SEG_KCODE << 3, vectors[i], 0);
    SETGATE(idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[T_SYSCALL], DPL_USER);

    initlock(&tickslock, "time");
}

void idtinit(void)
{
    lidt(idt, sizeof(idt));
}

//Call when a user process run into trouble.
//Will kill the porcess
#define USER_FAULT(fault_prompt)                                               \
    do                                                                         \
    {                                                                          \
        cprintf(fault_prompt);                                                 \
        cprintf("pid %d %s: trap %d err %d on cpu %d "                         \
                "eip 0x%x addr 0x%x--kill proc\n",                             \
                proc->pid, proc->name, tf->trapno, tf->err, cpunum(), tf->eip, \
                rcr2());                                                       \
        proc->killed = TRUE;                                                   \
    } while (0)

//Call this when kernel run into trouble
//Will panic the kernel with panic("trap").
#define KERN_FAULT                                                    \
    do                                                                \
    {                                                                 \
        cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n", \
                tf->trapno, cpunum(), tf->eip, rcr2());               \
        panic("trap");                                                \
    } while (0)

//PAGEBREAK: 41
void trap(struct trapframe *tf)
{
    if (tf->trapno == T_SYSCALL)
    {
        if (proc->killed)
            exit();
        proc->tf = tf;
        syscall();
        if (proc->killed)
            exit();
        return;
    }

    switch (tf->trapno)
    {
    case T_IRQ0 + IRQ_TIMER:
        if (cpunum() == 0)
        {
            acquire(&tickslock);
            ticks++;
            new_ticks++;
            wakeup(&ticks);
            release(&tickslock);
        }
        else if (cpunum() == 1)
            new_ticks++;
        lapiceoi();
        break;
    case T_IRQ0 + IRQ_IDE:
        ideintr();
        lapiceoi();
        break;
    case T_IRQ0 + IRQ_IDE + 1:
        // Bochs generates spurious IDE1 interrupts.
        break;
    case T_IRQ0 + IRQ_KBD:
        kbdintr(); //handle kbd
        lapiceoi();
        break;
    case T_IRQ0 + IRQ_COM1:
        uartintr();
        lapiceoi();
        break;
    case T_IRQ0 + 7:
    case T_IRQ0 + IRQ_SPURIOUS:
        cprintf("cpu%d: spurious interrupt at %x:%x\n",
                cpunum(), tf->cs, tf->eip);
        lapiceoi();
        break;
    
    //floating point error
    case T_DIVIDE:
        if (VALIDATE_HANDLER(proc->sighandlers[SIGFPE]))
        {
            sigsend(proc->pid, SIGFPE);
        }
        lapiceoi();
        break;

    //page fault
    case T_PGFLT:
        // Specially, if the trap 14 occurs with address=0x0, it could be a null dereference.
        if (proc != 0 && (tf->cs & 3) != 0) //is user proc
        {
            /*
            it's like this:
            STACKBASE->     ---------                   -\
                            |       |                     |
                            |       |                     |
                            |       |                     |
            stk_off->       ---------   -\                 > the whole stack size after extending is (STACKBASE-stk_off)
                            |       |     |               |
                            |       |      > PGSIZE       |
                            |       |     |               |
            stk_next_off->  ---------   -/              -/
             */


            int addr = rcr2();
            uint stk_off = STACKBASE - proc->stk_sz * PGSIZE;
            uint stk_next_off = STACKBASE - (proc->stk_sz + 1) * PGSIZE;

            //access to the 0x0 address indicates a null dereference.
            if (addr == 0x0)
            {
                USER_FAULT("Segment fault.\n");
            }
            //will not allocate more than MAX_PROC_STK_PGCNT pages for stack, otherwise the SEGFAULT will arise.
            else if (addr <= stk_off && addr >= stk_next_off && proc->stk_sz < MAX_PROC_STK_PGCNT)
            {
                if (allocuvm(proc->pgdir, stk_next_off, stk_off) == 0)
                {
                    USER_FAULT("Segment fault.\n"); //can't alloc more page
                }
                else
                {
                    proc->stk_sz += 1; //new page allocated.
                }
            }
            //not specified conditions, kill the proc.
            else
            {
                USER_FAULT("Segment fault.\n");
            }
        }
        else // In kernel
        {
            // In kernel, it must be our mistake.
            KERN_FAULT;
        }

        break;

    //PAGEBREAK: 13
    default:
        if (proc == 0 || (tf->cs & 3) == 0)
        {
            // In kernel, it must be our mistake.
            KERN_FAULT;
        }

        // In user space, assume process misbehaved.
        USER_FAULT("");
    }

    // Force process exit if it has been killed and is in user space.
    // (If it is still executing in the kernel, let it keep running
    // until it gets to the regular system call return.)
    if (proc && proc->killed && (tf->cs & 3) == DPL_USER)
        exit();

    // Force process to give up CPU on clock tick.
    // If interrupts were on while locks held, would need to check nlock.
    if (proc && proc->state == RUNNING && tf->trapno == T_IRQ0 + IRQ_TIMER)
        yield();

    // Check if the process has been killed since we yielded
    if (proc && proc->killed && (tf->cs & 3) == DPL_USER)
        exit();
}
