#ifndef PLOCK_H
#define PLOCK_H

#include "spinlock.h"

struct plock {
  struct spinlock lk;   // plock for intrnals
  int locked;           // 0 ,taken and 1 is free
  struct proc *owner;   // owner
  struct proc *head;    // first person in the queue
};

void plock_init(struct plock *pl);
void plock_acquire(struct plock *pl, int priority);
void plock_release(struct plock *pl);

extern struct plock global_plock;

#endif
