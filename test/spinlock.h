struct spinlock {
  int locked; // 0 if not locked, 1 if locked
};

void initlock(struct spinlock *);
void acquire(struct spinlock *);
void release(struct spinlock *);