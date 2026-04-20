#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000
#define PRINT_LIMIT 10

int A[MAX][MAX], B[MAX][MAX];
int Add[MAX][MAX], Sub[MAX][MAX];

double time_diff(struct timespec s, struct timespec e)
{
    return (e.tv_sec - s.tv_sec)*1000.0 +
           (e.tv_nsec - s.tv_nsec) / 1000000.0;
}

int main()
{
    int r, c;
    double a, s, t;

    printf("Enter number of rows and columns: ");
    scanf("%d %d", &r, &c);

    if (r > MAX || c > MAX)
    {
        printf("Matrix size exceeds limit!\n");
        return 1;
    }

    srand(time(NULL));

    /* Generate random matrices */
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
        {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    struct timespec add_start, add_end;
    struct timespec sub_start, sub_end;

    /* Serial Addition */
    clock_gettime(CLOCK_MONOTONIC, &add_start);

    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            Add[i][j] = A[i][j] + B[i][j];

    clock_gettime(CLOCK_MONOTONIC, &add_end);

    /* Serial Subtraction */
    clock_gettime(CLOCK_MONOTONIC, &sub_start);

    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            Sub[i][j] = A[i][j] - B[i][j];

    clock_gettime(CLOCK_MONOTONIC, &sub_end);

    /* Print results only for small matrices */
    if (r <= PRINT_LIMIT && c <= PRINT_LIMIT)
    {
        printf("\nMatrix Addition:\n");
        for (int i = 0; i < r; i++)
        {
            for (int j = 0; j < c; j++)
                printf("%4d ", Add[i][j]);
            printf("\n");
        }

        printf("\nMatrix Subtraction:\n");
        for (int i = 0; i < r; i++)
        {
            for (int j = 0; j < c; j++)
                printf("%4d ", Sub[i][j]);
            printf("\n");
        }
    }

    a=time_diff(add_start, add_end);
    s=time_diff(sub_start, sub_end);
    t=a+s;
    printf("\nSerial Addition execution time   : %.6f milliseconds",a);
    printf("\nSerial Subtraction execution time: %.6f milliseconds\n",s);
    printf("\nSerial total execution time: %.6f milliseconds\n",t);

    return 0;
}