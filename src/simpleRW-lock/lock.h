#ifndef _LOCK_H_
#define _LOCK_H_

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#ifdef __cplusplus
#define __restrict__
extern "C" {
#endif

  typedef struct ReaderWriterLock {
    volatile int64_t readers;
    volatile int writer;
  } ReaderWriterLock;


  //All lock acq and free operations require that the lock state not be acquired/free
  // by this thread before the call, undefined behavior if this is not the case
  // The benchmark does not test this precondition.

  //init the lock and any associated resources
  void rw_lock_init(ReaderWriterLock *rwlock);

  //called by reader thread to acquire the lock
  //stall on the lock, should guarantee eventual progress
  //thread_id is the CPU ID of the calling thread - for CADE, this is between 0-7
  //may not be necessary for your implementation but this value is passed to the lock by the benchmark.
  void read_lock(ReaderWriterLock *rwlock, uint8_t thread_id);

  //unlock a reader thread that has acquired the lock
  void read_unlock(ReaderWriterLock *rwlock, uint8_t thread_id);

  //acquire a write lock
  //stall until lock is acquired - guarantee eventual progress.
  void write_lock(ReaderWriterLock *rwlock);

  //free an acquired write lock
  void write_unlock(ReaderWriterLock *rwlock);

#ifdef __cplusplus
}
#endif
#endif
