#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#define INITIAL -1
#define THREAD_NUM 4
#define READ 0
#define WRITE 1

struct dataForCollectingThread
{
    pthread_t *taskThread;
    int **primePipes;
    int *primes;
    int pipesNum;
};

struct dataForPrimesThread
{
    int start;
    int end;
    int *pipe;
};

int cmp(const void *num1, const void *num2)
{
    int arg1 = *(const int *)num1;
    int arg2 = *(const int *)num2;

    if (arg1 < arg2)
        return -1;
    if (arg1 > arg2)
        return 1;
    return 0;
}

void *searchPrimes(void *i)
{
    struct dataForPrimesThread *data = (struct dataForPrimesThread *)i;
    int start = data->start;
    int end = data->end;
    int *pipe = data->pipe;
    free(data);
    for (int i = start; i < end; i++) // <b,e)
    {
        int isPrime = 1;
        if (i == 1)
            isPrime = 0;
        for (int j = 2; j <= sqrt(i); j++)
            if (i % j == 0)
                isPrime = 0;
        if (isPrime == 1)
            write(pipe[WRITE], &i, sizeof(int));
    }
    return NULL;
}

void subProcess(int start, int end, int id, int *pipe)
{
    int diff = end - start;
    pthread_t *threads = (pthread_t *)malloc(THREAD_NUM * sizeof(pthread_t)); // pod proces ma tablice wątków do synchronizacji
    int intervalSize = diff / THREAD_NUM;
    int currPos = start;
    close(pipe[READ]);
    for (int i = 0; i < THREAD_NUM; i++) // dzielimy na równe podprzedziały
    {
        struct dataForPrimesThread *data = (struct dataForPrimesThread *)malloc(sizeof(struct dataForPrimesThread));
        data->start = currPos;
        if (i == THREAD_NUM - 1)
            data->end = end;
        else
            data->end = currPos + intervalSize;
        data->pipe = pipe;
        pthread_create(&threads[i], NULL, searchPrimes, (void *)data);
        currPos += intervalSize;
    }
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(threads[i], NULL);
    free(threads);
    close(pipe[WRITE]);
}

void *collectPrimes(void *v)
{
    struct dataForCollectingThread *data = (struct dataForCollectingThread *)v;
    pthread_t *taskThread = data->taskThread;
    int **pipes = data->primePipes;
    int *primes = data->primes;
    int pipesNumber = data->pipesNum;
    free(data);

    int counter = 0;
    // double numLog = log(end);
    // int num = round((double)end / numLog);
    // int *primes = (int *)malloc(num * 2 * sizeof(int));

    for (int i = 0; i < pipesNumber; i++)
        close(pipes[i][WRITE]);

    int prime;

    for (int i = 0; i < pipesNumber; i++)
    {
        while (read(pipes[i][READ], &prime, sizeof(int)) > 0)
        {
            primes[counter] = prime;
            counter++;
        }
    }
    for (int i = 0; i < pipesNumber; i++)
        close(pipes[i][READ]);
    qsort(primes, counter, sizeof(int), cmp);

    for (int i = 0; i < counter; i++)
        printf("%d \n", primes[i]);

    *taskThread = 0;
    printf("Finished task \n");
    free(primes);
    for(int i=0; i<pipesNumber; i++)
        free(pipes[i]);
    free(pipes);
    return NULL;
}

void *createTask(int *task, pthread_t *taskThread, int ***primePipes, int **primes, int *pipesSize)
{
    int diff = task[1] - task[0];
    printf("%d %d %d \n", task[0], task[1], task[2]);
    if (diff < -1)
    {
        printf("Invalid interval \n");
        return (void *)EXIT_FAILURE;
    }
    if (task[0] < 1)
    {
        printf("Interval must start at least from 1 \n");
        return (void *)EXIT_FAILURE;
    }
    if (task[2] < 1)
    {
        printf("There must be at least one subprocess \n");
        return (void *)EXIT_FAILURE;
    }
    if (diff < task[2])
    {
        printf("There is more subProcesses than numbers in a interval \n");
        return (void *)EXIT_FAILURE;
    }

    int intervalSize = diff / task[2];
    int currpos = task[0];
    int id = 0;

    double numLog = log(task[1]);
    int num = round((double)task[1] / numLog);
    (*primes) = (int *)malloc(num * 2 * sizeof(int));

    (*pipesSize) = task[2];
    (*primePipes) = (int **)malloc(task[2] * sizeof(int *));

    for (int i = 0; i < task[2]; i++)
        (*primePipes)[i] = (int *)malloc(2 * sizeof(int));

    for (int i = 0; i < task[2]; i++)
    {
        if (pipe((*primePipes)[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < task[2]; i++)
    {
        int pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) // tworzenie pod procesów
        {
            if (currpos + intervalSize < task[1])
                subProcess(currpos, currpos + intervalSize, id, (*primePipes)[i]);
            else
                subProcess(currpos, task[1], id, (*primePipes)[i]);
            exit(0);
        }
        else
        {
            // printf("%d \n", pid);
            currpos += intervalSize;
            id++;
        }
    }
    struct dataForCollectingThread *newData = (struct dataForCollectingThread *)malloc(sizeof(struct dataForCollectingThread));
    newData->taskThread = taskThread;
    newData->primePipes = (*primePipes);
    newData->primes = (*primes);
    newData->pipesNum = task[2];
    pthread_create(taskThread, NULL, collectPrimes, (void *)newData); // wątek do odbioru danych (aby dało się wyjśc w trakcie obliczeń)
    printf("New task created\n");
    return NULL;
}

int main()
{
    char *myfifo = "./taskQueue";
    mkfifo(myfifo, 0666);
    int fd;
    char bufffer[80];
    pthread_t taskThread = 0;
    int **primePipes = NULL;
    int pipeSize = 0;
    // int primePipe[2];
    int *primes = NULL;
    int task[] = {INITIAL, INITIAL, INITIAL}; // start, end, num
    for (int i = 0; i < 80; i++)
        bufffer[i] = 0;
    while (1)
    {
        fd = open(myfifo, O_RDONLY);
        read(fd, bufffer, sizeof(bufffer));
        if (strcmp(bufffer, "q") == 0) // wyjście na siłę
        {
            for (int i = 0; i < pipeSize; i++)
            {
                close(primePipes[i][READ]);
                close(primePipes[i][WRITE]);
            }
            remove(myfifo);
            printf("Quiting\n");
            if (taskThread != 0)
                pthread_cancel(taskThread);
            exit(0);
        }
        if (bufffer[0] == 'n') // nowe zadanie
        {
            char *tok = strtok(bufffer, " ");
            int i = 0;
            while (tok != NULL)
            {
                tok = strtok(NULL, " ");
                if (tok != NULL && i < 3)
                    task[i] = atoi(tok);
                i++;
            }
            if (task[0] != INITIAL && task[1] != INITIAL && task[2] != INITIAL && i == 4)
            {
                if (taskThread == 0)
                    createTask(task, &taskThread, &primePipes, &primes, &pipeSize);
                else
                    printf("Task started, wait until it finishes or quit \n");
            }
            else
                printf("Failed to create a task\n");
        }
        // czyszczenie bufora i zadania
        bufffer[0] = '\0';
        bufffer[1] = '\0';
        task[0] = task[1] = task[2] = INITIAL;
        close(fd);
    }
}