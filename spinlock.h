// Mutual exclusion lock.
#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include "types.h"
#include "param.h" // NCPU

struct cpu;

struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging: owner tracking
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
  // per-CPU stats
  // number of times the cpu aquired the lock
  uint64 acq_count[NCPU];
  //spin-loop that the CPU waited
  uint64 total_spins[NCPU];
};

void initlock(struct spinlock*, char*);
void acquire(struct spinlock*);
void release(struct spinlock*);
int holding(struct spinlock*);

#endif
