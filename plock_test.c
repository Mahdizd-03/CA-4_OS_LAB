#include "types.h"
#include "stat.h"
#include "user.h"

void
child_work(int pri, int hold_ticks)
{
  int pid = getpid();
  printf(1,"pid %d pri %d: requesting plock\n", pid, pri);

  plock_acquire(pri);

  printf(1,"pid %d pri %d: acquired plock\n", pid, pri);

  sleep(hold_ticks);

  printf(1,"pid %d pri %d: releasing plock\n", pid, pri);
  plock_release();
  exit();
}

int
main(int argc, char *argv[])
{

  // low priority first
  if (fork() == 0)
    child_work(10, 200);

  sleep(1);

  // higher priorities
  if (fork() == 0) child_work(50, 50);
  if (fork() == 0) child_work(40, 50);
  if (fork() == 0) child_work(30, 50);
  if (fork() == 0) child_work(20, 50);

  while (wait()>0);

  printf(1,"plock_test: all children done\n");
  exit();
}
