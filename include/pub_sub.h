#ifndef PUB_SUB_H
#define PUB_SUB_H

#include <time.h>

int is_prime(int n);
void record_occupancy(void);
void *producer(void *arg);
void *consumer(void *arg);
double elapsed_ms(struct timespec start, struct timespec end);

#endif // PUB_SUB_H
