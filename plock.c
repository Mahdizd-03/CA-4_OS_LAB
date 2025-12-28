#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "plock.h"


struct plock global_plock;

void
plock_init(struct plock *pl)
{
  initlock(&pl->lk, "plock-lk");
  pl->locked = 0;
  pl->owner = 0;
  pl->head = 0;
}

static void
plock_remove(struct plock *pl, struct proc *p)
{
  struct proc *cur = pl->head;
  struct proc *prev = 0;
  while (cur) {
    if (cur == p) {
      if (prev)
        prev->pl_next = cur->pl_next;
      else
        pl->head = cur->pl_next;
      cur->pl_next = 0;
      cur->pl_waiting = 0;
      return;
    }
    prev = cur;
    cur = cur->pl_next;
  }
}

// try to acquire; if locked, enqueue current proc and sleep on its address. lager the higher the priority is

void
plock_acquire(struct plock *pl, int priority)
{
  struct proc *p = myproc();

  acquire(&pl->lk);
  if (!pl->locked) {
    pl->locked = 1;
    pl->owner = p;
    release(&pl->lk);
    return;
  }

  // otherwise, enqueue 
  p->pl_priority = priority;
  p->pl_next = pl->head;
  pl->head = p;
  p->pl_waiting = 1;

  // pointer to proc
  sleep(p, &pl->lk);

  // when woken up, this proc is the chosen next owner 
  pl->owner = p;
  p->pl_waiting = 0;
  // keep pl->locked == 1
  release(&pl->lk);
}

void
plock_release(struct plock *pl)
{
  acquire(&pl->lk);

  // if no owner or not locked, nothing to do
  if (!pl->locked) {
    release(&pl->lk);
    return;
  }

  // If no waiters, free the lock
  if (pl->head == 0) {
    pl->locked = 0;
    pl->owner = 0;
    release(&pl->lk);
    return;
  }

  // find the proc with max pri
  struct proc *cur = pl->head;
  struct proc *best = cur;
  int best_pr = cur->pl_priority;
  while (cur) {
    if (cur->pl_priority > best_pr) {
      best = cur;
      best_pr = cur->pl_priority;
    }
    cur = cur->pl_next;
  }

  // remove it
  plock_remove(pl, best);

  // set owner to best and wake only that proc 
  pl->owner = best;
  // pl->locked remains 1
  wakeup(best);   // pointer to proc
  release(&pl->lk);
}
