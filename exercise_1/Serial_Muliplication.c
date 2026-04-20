#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000
#define PRINT_LIMIT 10

int A[MAX][MAX], B[MAX][MAX], C[MAX][MAX];

int main() {
    int r1, c1, r2, c2;
    struct timespec start, end;

    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &r1, &c1);
    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &r2, &c2);

    if (c1 != r2) {
        printf("Matrix multiplication not possible!\n");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c1; j++)
            A[i][j] = rand() % 10;

    for (int i = 0; i < r2; i++)
        for (int j = 0; j < c2; j++)
            B[i][j] = rand() % 10;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            C[i][j] = 0;
            for (int k = 0; k < c1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken = (end.tv_sec - start.tv_sec)*1000.0 +
                        (end.tv_nsec - start.tv_nsec) / 1000000.0;

    if (r1 <= PRINT_LIMIT && c2 <= PRINT_LIMIT) {
        printf("\nResultant Matrix:\n");
        for (int i = 0; i < r1; i++) {
            for (int j = 0; j < c2; j++)
                printf("%5d ", C[i][j]);
            printf("\n");
        }
    }

    printf("\nExecution time: %.9f milliseconds\n", time_taken);

    return 0;
}
