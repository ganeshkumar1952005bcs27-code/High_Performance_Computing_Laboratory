#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_QUEUE 100
#define MAX_LINE 256

char queue[MAX_QUEUE][MAX_LINE];
int front = 0, rear = 0, count = 0;

int done_producers = 0;
int NUM_PRODUCERS, NUM_CONSUMERS;

omp_lock_t lock;

void enqueue(char *line) {
    while (1) {
        omp_set_lock(&lock);

        if (count < MAX_QUEUE) {
            strcpy(queue[rear], line);
            rear = (rear + 1) % MAX_QUEUE;
            count++;
            omp_unset_lock(&lock);
            break;
        }

        omp_unset_lock(&lock);
    }
}

int dequeue(char *line) {
    int flag = 0;

    omp_set_lock(&lock);

    if (count > 0) {
        strcpy(line, queue[front]);
        front = (front + 1) % MAX_QUEUE;
        count--;
        flag = 1;
    }

    omp_unset_lock(&lock);
    return flag;
}

void producer(int id, char *filename, FILE *fp) {
    FILE *f = fopen(filename, "r");

    if (!f) {
        printf("Producer %d: Cannot open %s\n", id, filename);
        return;
    }

    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, f)) {
        enqueue(line);

        #pragma omp critical
        {
            printf("Producer %d produced: %s", id, line);
            fprintf(fp, "Producer %d: %s", id, line);
        }
    }

    fclose(f);

    #pragma omp atomic
    done_producers++;
}

void consumer(int id, FILE *fp) {
    char line[MAX_LINE];

    while (1) {
        if (dequeue(line)) {

            char *token = strtok(line, " \t\n");

            while (token != NULL) {
                #pragma omp critical
                {
                    printf("Consumer %d consumed: %s\n", id, token);
                    fprintf(fp, "Consumer %d: %s\n", id, token);
                }

                token = strtok(NULL, " \t\n");
            }

        } else {
            #pragma omp flush(done_producers)
            if (done_producers == NUM_PRODUCERS)
                break;
        }
    }
}

int main() {
    printf("Enter number of producers: ");
    scanf("%d", &NUM_PRODUCERS);

    printf("Enter number of consumers: ");
    scanf("%d", &NUM_CONSUMERS);

    // File names (must exist)
    char *files[] = {"file1.txt", "file2.txt", "file3.txt"};

    FILE *fp = fopen("pc_output.txt", "w");

    omp_init_lock(&lock);

    double start = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_PRODUCERS + NUM_CONSUMERS)
    {
        int tid = omp_get_thread_num();

        if (tid < NUM_PRODUCERS) {
            producer(tid, files[tid], fp);
        } else {
            consumer(tid - NUM_PRODUCERS, fp);
        }
    }

    double end = omp_get_wtime();

    omp_destroy_lock(&lock);
    fclose(fp);

    printf("\nExecution Time = %f seconds\n", end - start);

    return 0;
}