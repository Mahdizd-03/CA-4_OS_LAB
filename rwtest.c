#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i;

  for(i = 0; i < 4; i++){
    if(fork() == 0){
      rwtest(0); // reader
      exit();
    }
  }

  if(fork() == 0){
    rwtest(1); // writer
    exit();
  }

  // parent waits 
  for(i = 0; i < 5; i++)
    wait();

  exit();
}
