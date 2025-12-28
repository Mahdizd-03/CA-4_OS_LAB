#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"   

int
main(int argc, char *argv[])
{
  int i, j;
  uint64 scores[NCPU];

  // base
  for(i = 0; i < NCPU; i++) scores[i] = 0;
  if(getlockstat((uint64)scores, 0) < 0){
    printf(1, "getlockstat failed\n");
    exit();
  }

  printf(1, "Initial scores (per-mille):\n");
  for(i = 0; i < NCPU; i++){
    printf(1, "cpu %d: %d\n", i, (int)scores[i]);
  }

  // child
for(i = 0; i < 4; i++){
  if(fork() == 0){
    int k;
    for(k = 0; k < 2000; k++)
      uptime();  
    exit();
  }
}
while(wait() > 0);

  // final snapshot
  for(i = 0; i < NCPU; i++) scores[i] = 0;
  if(getlockstat((uint64)scores, 0) < 0){
    printf(1, "getlockstat failed\n");
    exit();
  }

  printf(1, "Final scores (per-mille):\n");
  for(i = 0; i < NCPU; i++){
    printf(1, "cpu %d: %d\n", i, (int)scores[i]);
  }

  exit();
}
