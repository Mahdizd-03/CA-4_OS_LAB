#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"
#include "rwlock.h"
#include "plock.h"

extern uint ticks;
extern struct spinlock tickslock;
extern void rwlock_test(int, int);
//sys_fork has changed due to testing rwlock. original is commented
int
sys_fork(void)
{
  return fork();
}
//int
//sys_fork(void)
//{
//  int pid = fork();

//  if(pid == 0){
//    struct proc *p = myproc();
//    rwlock_test(p->pid, p->pid % 4 == 0);
//  }

//  return pid;
//}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


extern struct rwlock testlock;
// test
int
sys_rwtest(void)
{
  int is_writer;

  if(argint(0, &is_writer) < 0)
    return -1;

  if(is_writer){
    rwlock_acquire_write(&testlock);
    cprintf("WRITER %d entered\n", myproc()->pid);

    acquire(&tickslock);
    uint start = ticks;
    while(ticks - start < 100){
      sleep(&ticks, &tickslock);
    }
    release(&tickslock);

    cprintf("WRITER %d leaving\n", myproc()->pid);
    rwlock_release_write(&testlock);
  } else {
    rwlock_acquire_read(&testlock);
    cprintf("READER %d entered\n", myproc()->pid);

    acquire(&tickslock);
    uint start = ticks;
    while(ticks - start < 100){
      sleep(&ticks, &tickslock);
    }
    release(&tickslock);

    cprintf("READER %d leaving\n", myproc()->pid);
    rwlock_release_read(&testlock);
  }

  return 0;
}

// func
int
sys_getlockstat(void)
{
  uint user_addr;
  if(argint(0, (int *)&user_addr) < 0)
    return -1;

  extern struct spinlock tickslock;
  struct spinlock *lk = &tickslock;

  uint64 scores[NCPU];
  int i;

  for(i = 0; i < NCPU; i++){
    uint a = (uint)lk->acq_count[i];
    uint s = (uint)lk->total_spins[i];


 //   cprintf("cpu %d: acq=%d spins=%d\n", i, a, s);

    if(a == 0)
      scores[i] = 0;
    else
      scores[i] = (s * 1000) / a;   // avg spins per acquire 
  }

  if(copyout(myproc()->pgdir,
             user_addr,
             (char *)scores,
             sizeof(scores)) < 0)
    return -1;

  return 0;
}

int
sys_plock_acquire(void)
{
  int priority;
  if (argint(0, &priority) < 0)
    return -1;
  plock_acquire(&global_plock, priority);
  return 0;
}

int
sys_plock_release(void)
{
  plock_release(&global_plock);
  return 0;
}

