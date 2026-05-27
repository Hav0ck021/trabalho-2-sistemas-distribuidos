#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdatomic.h>

#include "pub_sub.h"

#define MAX_HISTORY 2000000

typedef struct
{
    int id;
} thread_arg_t;

// shared buffer
int *buffer;
int buffer_size;

// circular buffer indexes
int in_index = 0;
int out_index = 0;

// current occupancy
int occupancy = 0;

// semaphores
sem_t empty_slots;
sem_t full_slots;
sem_t mutex;

// execution control
atomic_int consumed_count = 0;
atomic_int stop_flag = 0;

long max_items;

// occupancy history
int *history;
atomic_int history_index = 0;

// primality test
int is_prime(int n)
{
    if (n < 2)
        return 0;
    if (n == 2)
        return 1;
    if (n % 2 == 0)
        return 0;

    for (int i = 3; i * i <= n; i += 2)
    {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

// save occupancy state
void record_occupancy(void)
{
    int idx = atomic_fetch_add(&history_index, 1);

    if (idx < MAX_HISTORY)
        history[idx] = occupancy;
}

// producer thread
void *producer(void *arg)
{
    (void) arg;

    while (!atomic_load(&stop_flag))
    {
        int value = (rand() % 10000000) + 1;
        sem_wait(&empty_slots);
        
        if (atomic_load(&stop_flag))
        {
            sem_post(&empty_slots);
            break;
        }

        sem_wait(&mutex);

        buffer[in_index] = value;
        in_index = (in_index + 1) % buffer_size;

        occupancy++;

        record_occupancy();

        sem_post(&mutex);
        sem_post(&full_slots);
    }

    return NULL;
}

// consumer thread
void *consumer(void *arg)
{

    (void) arg;
    while (1)
    {
        sem_wait(&full_slots);

        if (atomic_load(&stop_flag))
        {
            sem_post(&full_slots);
            break;
        }

        sem_wait(&mutex);

        int value = buffer[out_index];

        out_index = (out_index + 1) % buffer_size;

        occupancy--;

        record_occupancy();

        sem_post(&mutex);
        sem_post(&empty_slots);

        int current = atomic_fetch_add(&consumed_count, 1);

        if (current >= max_items)
        {
            atomic_store(&stop_flag, 1);

            // unlock possible blocked threads
            for (int i = 0; i < 1000; i++)
            {
                sem_post(&empty_slots);
                sem_post(&full_slots);
            }
            break;
        }
        is_prime(value);
    }
    return NULL;
}

double elapsed_ms(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage:\n");
        printf("%s <buffer_size> <producers> <consumers> <max_items>\n", argv[0]);
        return 1;
    }

    buffer_size = atoi(argv[1]);

    int producer_count = atoi(argv[2]);
    int consumer_count = atoi(argv[3]);

    max_items = atol(argv[4]);

    buffer = malloc(buffer_size * sizeof(int));

    if (buffer == NULL)
    {
        printf("Buffer allocation error.\n");
        return 1;
    }

    history = malloc(MAX_HISTORY * sizeof(int));

    if (history == NULL)
    {
        printf("History allocation error.\n");
        return 1;
    }

    srand(time(NULL));

    // semaphore initialization
    sem_init(&empty_slots, 0, buffer_size);
    sem_init(&full_slots, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_t producers[producer_count];
    pthread_t consumers[consumer_count];

    thread_arg_t producer_args[producer_count];
    thread_arg_t consumer_args[consumer_count];

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // create producer threads
    for (int i = 0; i < producer_count; i++)
    {
        producer_args[i].id = i;
        pthread_create(&producers[i], NULL, producer, &producer_args[i]);
    }

    // create consumer threads
    for (int i = 0; i < consumer_count; i++)
    {
        consumer_args[i].id = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_args[i]);
    }

    // wait producers
    for (int i = 0; i < producer_count; i++)
    {
        pthread_join(producers[i], NULL);
    }

    // wait consumers
    for (int i = 0; i < consumer_count; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = elapsed_ms(start, end);

    printf("Execution finished.\n");
    printf("Consumed items: %d\n", atomic_load(&consumed_count));
    printf("Elapsed time: %.3f ms\n", elapsed);

    // save occupancy history
    FILE *fp = fopen("buffer_occupancy.csv", "w");

    if (fp != NULL)
    {
        fprintf(fp, "operation,occupancy\n");
        int total = atomic_load(&history_index);

        for (int i = 0; i < total; i++)
            fprintf(fp, "%d,%d\n", i, history[i]);
        fclose(fp);
    }

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);

    free(buffer);
    free(history);

    return 0;
}