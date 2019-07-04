#if !defined(__INCLUDE_XV6_SLEPPLOCK_H)
#define __INCLUDE_XV6_SLEPPLOCK_H
#include <xv6/spinlock.h>

// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

#endif // __INCLUDE_XV6_SLEPPLOCK_H
