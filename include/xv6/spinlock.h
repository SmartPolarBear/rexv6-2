#if !defined(__INCLUDE_XV6_SPINLOCK_H)
#define __INCLUDE_XV6_SPINLOCK_H

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  const char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};


#if defined(__cplusplus)
extern "C"
{
#endif

    void acquire(struct spinlock *);
    void getcallerpcs(void *, uint *);
    int holding(struct spinlock *);
    void initlock(struct spinlock *,const char *);
    void release(struct spinlock *);
    void pushcli(void);
    void popcli(void);

#if defined(__cplusplus)
}
#endif
#endif // __INCLUDE_XV6_SPINLOCK_H
