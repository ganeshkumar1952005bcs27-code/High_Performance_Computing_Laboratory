#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX 1000
#define PRINT_LIMIT 10

int A[MAX][MAX], B[MAX][MAX];
int Add[MAX][MAX], Sub[MAX][MAX];
double sub,add,total=0;

double time_diff(struct timespec s, struct timespec e)
{
    return (e.tv_sec - s.tv_sec)*1000.0 +
           (e.tv_nsec - s.tv_nsec) / 1000000.0;
}

int main()
{
    int r, c;
    pid_t pid;

    printf("Enter number of rows and columns: ");
    scanf("%d %d", &r, &c);

    if (r > MAX || c > MAX)
    {
        printf("Matrix size exceeds limit!\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
        {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    pid = fork();

    if (pid == 0)   // Child process → Subtraction
    {
        struct timespec cs, ce;
        clock_gettime(CLOCK_MONOTONIC, &cs);

        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                Sub[i][j] = A[i][j] - B[i][j];

        clock_gettime(CLOCK_MONOTONIC, &ce);

        if (r <= PRINT_LIMIT && c <= PRINT_LIMIT)
        {
            printf("\nChild Process (PID %d): Matrix Subtraction\n", getpid());
            for (int i = 0; i < r; i++)
            {
                for (int j = 0; j < c; j++)
                    printf("%4d ", Sub[i][j]);
                printf("\n");
            }
        }
        sub=time_diff(cs, ce);
        printf("\nChild (Subtraction) execution time: %.6f milliseconds\n",
               sub);
        if(total<sub)
           total=sub;

        exit(0);
    }
    else            // Parent process → Addition
    {
        struct timespec ps, pe;
        clock_gettime(CLOCK_MONOTONIC, &ps);

        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                Add[i][j] = A[i][j] + B[i][j];

        clock_gettime(CLOCK_MONOTONIC, &pe);

        wait(NULL);

        if (r <= PRINT_LIMIT && c <= PRINT_LIMIT)
        {
            printf("\nParent Process (PID %d): Matrix Addition\n", getpid());
            for (int i = 0; i < r; i++)
            {
                for (int j = 0; j < c; j++)
                    printf("%4d ", Add[i][j]);
                printf("\n");
            }
        }
        add=time_diff(ps, pe);
        printf("\nParent (Addition) execution time: %.6f milliseconds\n",
               add);
        if(total<add)
           total=add;
    }
    printf("Total execution time:%.6f milliseconds\n",total);

    return 0;
}