
#include "spinlock.h"

extern void initlock(struct spinlock *lk) {
  lk->locked = 0;
}

// Acquire the spinlock
extern void acquire(struct spinlock *lk) {
  while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ; // Spin until the lock is acquired
}

// Release the spinlock
extern void release(struct spinlock *lk) {
  __sync_lock_release(&lk->locked);
}