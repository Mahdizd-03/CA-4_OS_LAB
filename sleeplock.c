#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
//for test
struct sleeplock test_sleeplock;

void
initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, name);
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
  lk->owner = 0; // owner shit
}

void
acquiresleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  while (lk->locked) {
    sleep(lk, &lk->lk);
  }
  lk->locked = 1;
  lk->owner = myproc();
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

void
releasesleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  if(lk->owner != myproc()){
    cprintf("releasesleep: pid %d tried to release lock owned by pid %d\n",
           myproc() ? myproc()->pid : -1,
           lk->owner ? lk->owner->pid : -1);
    panic ("releasesleep: not owner");
  }
  lk->owner = 0;
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

int
holdingsleep(struct sleeplock *lk)
{
  int r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}
//for test
void
test_sleeplock_owner(void)
{
  int pid;

  cprintf("=== sleeplock owner test start ===\n");

  initsleeplock(&test_sleeplock, "test_sleeplock");

  // parent acquires the lock
  acquiresleep(&test_sleeplock);
  cprintf("Parent (pid=%d) acquired sleeplock\n", myproc()->pid);

  pid = fork();

  if(pid == 0){
    cprintf("Child (pid=%d) attempting illegal release...\n", myproc()->pid);

    // panic
    releasesleep(&test_sleeplock);

    cprintf("ERROR: child released sleeplock (this should not happen)\n");
    exit();
  }

  // parent waits and kernel should panic before this completes)
  wait();

  // clean
  releasesleep(&test_sleeplock);
  cprintf("=== sleeplock owner test end ===\n");
}



