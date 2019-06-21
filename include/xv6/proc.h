/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-22 00:05:22
 * @ Description:
 */

#ifndef __INCLUDE_XV6_PROC_H
#define __INCLUDE_XV6_PROC_H
#include <xv6/types.h>
#include <xv6/signal.h>
#include <xv6/x86.h>
#include <xv6/mmu.h>
#include <xv6/spinlock.h>

// Per-CPU state
struct cpu
{
  uchar apicid;              // Local APIC ID
  struct context *scheduler; // swtch() here to enter scheduler
  struct taskstate ts;       // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS]; // x86 global descriptor table
  volatile uint started;     // Has the CPU started?
  int ncli;                  // Depth of pushcli nesting.
  int intena;                // Were interrupts enabled before pushcli?

  // Cpu-local storage variables; see below
  struct cpu *cpu;
  struct proc *proc; // The currently-running process.
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// Per-CPU variables, holding pointers to the
// current cpu and to the current process.
// The asm suffix tells gcc to use "%gs:0" to refer to cpu
// and "%gs:4" to refer to proc.  seginit sets up the
// %gs segment register so that %gs refers to the memory
// holding those two variables in the local cpu's struct cpu.
// This is similar to how thread-local variables are implemented
// in thread libraries such as Linux pthreads.
extern struct cpu *cpu asm("%gs:0");   // &cpus[cpunum()]
extern struct proc *proc asm("%gs:4"); // cpus[cpunum()].proc

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context
{
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

// enum procstate
// {
//   UNUSED,
//   EMBRYO,
//   SLEEPING,
//   RUNNABLE,
//   RUNNING,
//   ZOMBIE
// };

enum procstate
{
  UNUSED,
  NEG_UNUSED,
  EMBRYO,
  SLEEPING,
  NEG_SLEEPING,
  RUNNABLE,
  NEG_RUNNABLE,
  RUNNING,
  ZOMBIE,
  NEG_ZOMBIE
};

typedef struct cstackframe
{
  int dest;
  int signum;
  BOOL used;
  struct cstackframe *next;
} cstackframe_t;

typedef struct cstack
{
  cstackframe_t frames[SIGNAL_MAX - SIGNAL_MIN];
  cstackframe_t *head;
} cstack_t;

typedef struct
{
  int isfree;
  struct spinlock lock;
  int flag;   //1 when locked
  void *cond; //conditional
} mutex_t;

typedef struct
{

} cond_t;

// Per-process state
struct proc
{
  uint sz;                    // Size of process memory (bytes)
  uint stk_sz;                // Size of stack in page
  pde_t *pgdir;               // Page table
  char *kstack;               // Bottom of kernel stack for this process
  enum procstate state;       // Process state
  int pid;                    // Process ID
  struct proc *parent;        // Parent process
  struct trapframe *tf;       // Trap frame for current syscall
  struct context *context;    // swtch() here to run process
  void *chan;                 // If non-zero, sleeping on chan
  int killed;                 // If non-zero, have been killed
  struct file *ofile[NOFILE]; // Open files
  struct inode *cwd;          // Current directory
  char name[16];              // Process name (debugging)
  int tickets;                //Ticket count for lottery scheduling
  unsigned long long starttime;

  // Multi-thread support
  void *ustack;  // Bottom of the user stack
  BOOL isthread; // If non-zero, it's the main thread of a process
  struct proc *joinedthread;
  void *retval;

  //Mutex
  mutex_t mtable[32];            //mutex table
  mutex_t *mtable_shared;        //shared among threads
  struct spinlock mlock;         //mutex table
  struct spinlock *mlock_shared; //shared among thread

  //Signal
  sighandler_t sighandlers[SIGNAL_COUNT];
  cstack_t cstack;
  BOOL ignore_signals;
  struct trapframe oldtf;
  BOOL sigpause_involked;
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   expandable heap
//   * expandable stack

#define DEFAULT_TICKETS (10)
#define MIN_PROC_NUM (2)
#define MAX_PROC_STK_SIZE (0x800000) //8MB,8388608B
#define MAX_PROC_STK_PGCNT (MAX_PROC_STK_SIZE / PGSIZE)

sighandler_t sigset(int signum, sighandler_t sighandler);
int sigsend(int pid, int signum);
int lastproc_pid(void);
void sigret(void);
void sigpause(void);
void handle_signals(struct trapframe *tf);

#endif