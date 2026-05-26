#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sum.h"

int8_t *vector;
long long global_sum = 0;
spinlock_t lock;

void acquire(spinlock_t *lock)
{
    // busy wait
    while (atomic_flag_test_and_set(&lock->flag)){}
}

void release(spinlock_t *lock)
{
    atomic_flag_clear(&lock->flag);
}

void *worker(void *arg)
{
    thread_arg_t *t = (thread_arg_t *) arg;
    long long local_sum = 0;

    for (long i = t->begin; i < t->end; i++)
        local_sum += vector[i];

    acquire(&lock);
    global_sum += local_sum;
    release(&lock);

    return NULL;
}

double time_ms(struct timespec begin, struct timespec end)
{
    return (end.tv_sec - begin.tv_sec) * 1000.0 + (end.tv_nsec - begin.tv_nsec) / 1e6;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s <N> <K>\n", argv[0]);
        return 1;
    }

    long N = atol(argv[1]);
    int K = atoi(argv[2]);
    vector = malloc(N * sizeof(int8_t));

    if (vector == NULL)
    {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    srand(time(NULL));

    // preenchimento aleatório
    for (long i = 0; i < N; i++)
        vector[i] = (rand() % 201) - 100;
    

    // soma sequencial para verificação
    long long soma_sequencial = 0;
    for (long i = 0; i < N; i++)
        soma_sequencial += vector[i];

    pthread_t threads[K];
    thread_arg_t args[K];

    atomic_flag_clear(&lock.flag);

    struct timespec begin, end;

    clock_gettime(CLOCK_MONOTONIC, &begin);

    long bloco = N / K;

    for (int i = 0; i < K; i++)
    {
        args[i].id = i;
        args[i].begin = i * bloco;

        if (i == K - 1)
            args[i].end = N;
        else
            args[i].end = (i + 1) * bloco;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < K; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time = time_ms(begin, end);

    if (global_sum != soma_sequencial)
    {
        printf("ERRO!\n");
        printf("Sequencial: %lld\n", soma_sequencial);
        printf("Threads: %lld\n", global_sum);
    } else {
        printf("OK\n");
        printf("Soma: %lld\n", global_sum);
        printf("Tempo: %.3f ms\n", time);
    }

    free(vector);
    return 0;
}