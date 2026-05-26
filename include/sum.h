#ifndef SUM_H
#define SUM_H

#include <stdint.h>
#include <stdatomic.h>
#include <time.h>

typedef struct {
	atomic_flag flag;
} spinlock_t;

typedef struct {
	int id;
	long begin;
	long end;
} thread_arg_t;

extern int8_t *vector;
extern long long global_sum;
extern spinlock_t lock;

void acquire(spinlock_t *lock);
void release(spinlock_t *lock);
void *worker(void *arg);
double time_ms(struct timespec begin, struct timespec end);

#endif // SUM_H
