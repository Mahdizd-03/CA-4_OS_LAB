#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "spinlock.h"

struct rwlock {
  struct spinlock lk;   // local vars lock
  char *name;
  int read_count;       // active readers
  int writer;           // is the writer in?
};

void rwlock_init(struct rwlock *rw, char *name);
void rwlock_acquire_read(struct rwlock *rw);
void rwlock_release_read(struct rwlock *rw);
void rwlock_acquire_write(struct rwlock *rw);
void rwlock_release_write(struct rwlock *rw);

#endif
