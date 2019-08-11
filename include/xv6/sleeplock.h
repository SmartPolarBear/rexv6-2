#if !defined(__INCLUDE_XV6_SLEEPLOCK_H)
#define __INCLUDE_XV6_SLEEPLOCK_H
#if defined(__cplusplus)
extern "C"
{
#endif
// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

#if defined(__cplusplus)
}
#endif
#endif // __INCLUDE_XV6_SLEEPLOCK_H
