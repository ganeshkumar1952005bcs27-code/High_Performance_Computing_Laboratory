#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX 1000
#define ROWS_PER_CHILD 10
#define PRINT_LIMIT 10

int A[MAX][MAX], B[MAX][MAX], C[MAX][MAX];

double time_diff(struct timespec s, struct timespec e)
{
    return (e.tv_sec - s.tv_sec)*1000.0 +
           ((e.tv_nsec - s.tv_nsec) / 1000000.0);
}

int main()
{
    int r1, c1, r2, c2;
    pid_t pid;

    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &r1, &c1);

    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &r2, &c2);

    if (c1 != r2)
    {
        printf("Matrix multiplication not possible!\n");
        exit(1);
    }

    srand(time(NULL));

    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c1; j++)
            A[i][j] = rand() % 10;

    for (int i = 0; i < r2; i++)
        for (int j = 0; j < c2; j++)
            B[i][j] = rand() % 10;

    int num_children = (r1 + ROWS_PER_CHILD - 1) / ROWS_PER_CHILD;
    int pipes[num_children][2];

    struct timespec total_start, total_end;
    clock_gettime(CLOCK_MONOTONIC, &total_start);

    for (int c = 0; c < num_children; c++)
    {
        pipe(pipes[c]);
        pid = fork();

        if (pid == 0)
        {
            close(pipes[c][0]);

            struct timespec cs, ce;
            clock_gettime(CLOCK_MONOTONIC, &cs);

            int start = c * ROWS_PER_CHILD;
            int rows = ROWS_PER_CHILD;
            if (start + rows > r1)
                rows = r1 - start;

            write(pipes[c][1], &rows, sizeof(int));

            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < c2; j++)
                {
                    C[start + i][j] = 0;
                    for (int k = 0; k < c1; k++)
                        C[start + i][j] += A[start + i][k] * B[k][j];
                }
                write(pipes[c][1], C[start + i], c2 * sizeof(int));
            }

            clock_gettime(CLOCK_MONOTONIC, &ce);
            printf("Child PID %d computed %d rows in %.6f milliseconds\n",
                   getpid(), rows, time_diff(cs, ce));

            close(pipes[c][1]);
            exit(0);
        }
    }

    for (int c = 0; c < num_children; c++)
    {
        close(pipes[c][1]);
        int rows;
        int start = c * ROWS_PER_CHILD;

        read(pipes[c][0], &rows, sizeof(int));
        for (int i = 0; i < rows; i++)
            read(pipes[c][0], C[start + i], c2 * sizeof(int));

        close(pipes[c][0]);
    }

    for (int i = 0; i < num_children; i++)
        wait(NULL);

    clock_gettime(CLOCK_MONOTONIC, &total_end);

    printf("\nTotal execution time: %.6f  milliseconds\n",
           time_diff(total_start, total_end));

    if (r1 <= PRINT_LIMIT && c2 <= PRINT_LIMIT)
    {
        printf("\nResultant Matrix:\n");
        for (int i = 0; i < r1; i++)
        {
            for (int j = 0; j < c2; j++)
                printf("%5d ", C[i][j]);
            printf("\n");
        }
    }

    return 0;
}