#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<time.h>

#define BUFFER_SIZE 5
#define NUM_THREADS 4

typedef struct {
    int a, b;
    char op;
} Task;

Task buffer[BUFFER_SIZE];
int in = 0, out = 0;
int done = 0;

sem_t empty, full, mutex;
FILE *fp;

int calculate(int a, int b, char op, int *valid) {
    *valid = 1;
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) { *valid = 0; return 0; }
            return a / b;
        default: *valid = 0; return 0;
    }
}

void* producer(void* arg) {

    char ops[] = {'+', '-', '*', '/'};
    srand(time(NULL));
    for(int i = 0; i < 5; i++) {
        Task t;
        t.a = rand() % 100;
        t.b = rand() % 100;
        t.op = ops[rand() % 4];

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = t;
        printf("Produced: %d %c %d\n", t.a, t.op, t.b);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);
        sleep(1);
    }

    // Signal completion
    sem_wait(&mutex);
    done = 1;
    sem_post(&mutex);

    // Wake up all consumers to check the 'done' flag
    for(int i = 0; i < NUM_THREADS; i++) {
        sem_post(&full);
    }

    return NULL;
}

void* consumer(void* arg) {
    int id = *((int*)arg);

    while(1) {
        sem_wait(&full);
        sem_wait(&mutex);

        // Check if producer is done and buffer is empty
        if (done && in == out) {
            sem_post(&mutex);
            break;
        }

        Task t = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

        int valid;
        int result = calculate(t.a, t.b, t.op, &valid);

        if(valid) {
            printf("Thread %d: %d %c %d = %d\n", id, t.a, t.op, t.b, result);
            fprintf(fp, "Thread %d: %d %c %d = %d\n", id, t.a, t.op, t.b, result);
        } else {
            printf("Thread %d: Invalid operation (%d %c %d)\n", id, t.a, t.op, t.b);
            fprintf(fp, "Thread %d: Invalid operation (%d %c %d)\n", id, t.a, t.op, t.b);
        }
        fflush(fp);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons[NUM_THREADS];
    int ids[NUM_THREADS];

    fp = fopen("result.txt", "w");
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&prod, NULL, producer, NULL);

    for(int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }

    // Wait for everyone to finish
    pthread_join(prod, NULL);
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(cons[i], NULL);
    }

    fclose(fp);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    printf("All threads finished. Results saved to result.txt\n");
    return 0;
}