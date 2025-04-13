#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

static int counter = 0;
int *primes;
int *stats;

struct dataForThread
{
    int begin;
    int end;
    int id;
    pthread_mutex_t *mut;
};

int cmp(const void *num1, const void *num2)
{
    int arg1 = *(const int *)num1;
    int arg2 = *(const int *)num2;

    if (arg1 < arg2)
        return 1;
    if (arg1 > arg2)
        return -1;
    return 0;
}

void *f(void *i)
{
    struct dataForThread *inter = (struct dataForThread *)i;
    int begin;
    int end;
    int id;
    pthread_mutex_t *mut;
    if (inter != NULL)
    {
        begin = inter->begin;
        end = inter->end;
        id = inter->id;
        mut = inter->mut;
        free(i);
    }
    else
    {
        printf("Data is invalid \n");
        return (void *)1;
    }

    for (int i = begin; i < end; i++) // <b,e)
    {
        int isPrime = 1;
        if (i == 1)
            isPrime = 0;
        for (int j = 2; j <= sqrt(i); j++)
            if (i % j == 0)
                isPrime = 0;
        if (isPrime == 1)
        {
            pthread_mutex_lock(mut);
            primes[counter] = i;
            counter++;
            stats[id]++;
            FILE *fptr;
            fptr = fopen("results.txt", "a");
            fprintf(fptr, "Thread: %d has found prime number: %d \n", id, i);
            fclose(fptr);
            pthread_mutex_unlock(mut);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int threadNumber; // liczba wątków
    int lowerBound;   // dolna granica
    int upperBound;   // górna granica
    if (argc == 4)
    {
        threadNumber = atoi(argv[1]);
        lowerBound = atoi(argv[2]);
        upperBound = atoi(argv[3]);
        if (upperBound < lowerBound) // <l, u>
        {
            printf("Invalid interval \n");
            return 1;
        }
        if (lowerBound < 1)
        {
            printf("Interval must start at least from 1 \n");
            return 1;
        }
        if (threadNumber < 1)
        {
            printf("There must be at least one thread \n");
            return 1;
        }
        if (upperBound - lowerBound < threadNumber)
        {
            printf("There is more threads than number in a interval \n");
            return 1;
        }
    }
    else
    {
        printf("Wrong argument number \n");
        return 1;
    }
    stats = (int *)malloc(threadNumber * sizeof(int));
    double numLog = log(upperBound);
    // printf("%f \n", numLog);
    int num = round((double)upperBound / numLog);
    // printf("%d \n", num);
    primes = (int *)malloc(num * 2 * sizeof(int));

    pthread_t *threads = (pthread_t *)malloc(threadNumber * sizeof(pthread_t));

    pthread_mutex_t mut;

    pthread_mutex_init(&mut, NULL);

    int diff = (upperBound - lowerBound) / 2;

    int currPos = lowerBound;
    for (int i = 0; i < threadNumber; i++)
    {
        struct dataForThread *data = (struct dataForThread *)malloc(sizeof(struct dataForThread));
        data->begin = currPos;
        if (i == threadNumber - 1)
            data->end = upperBound;
        else
            data->end = currPos + diff;
        currPos += diff;
        diff /= 2;
        data->id = i;
        data->mut = &mut;
        pthread_create(&threads[i], NULL, f, (void *)data);
    }

    for (int i = 0; i < threadNumber; i++)
        pthread_join(threads[i], NULL);

    // posortować liczby na koniec
    qsort(primes, counter, sizeof(int), cmp);

    for (int i = 0; i < counter; i++)
        printf("%d \n", primes[i]);
    for (int i = 0; i < threadNumber; i++)
        printf("Thread %d has found %d prime numbers \n", i, stats[i]);

    pthread_mutex_destroy(&mut);
    free(threads);
    free(primes);
    free(stats);
}