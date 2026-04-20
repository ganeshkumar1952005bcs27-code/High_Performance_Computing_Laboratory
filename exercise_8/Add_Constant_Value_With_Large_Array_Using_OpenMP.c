#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main() {
    int n, i, constant;
    double start, end;

    printf("Enter the size of array: ");
    scanf("%d", &n);

    printf("Enter the constant value to add: ");
    scanf("%d", &constant);

    int *A = (int *)malloc(n * sizeof(int));
    int *B = (int *)malloc(n * sizeof(int));

    srand(time(0));
    for (i = 0; i < n; i++) {
        A[i] = rand() % 100;   // values 0–99
    }

    FILE *fp = fopen("output_constant.txt", "w");

    start = omp_get_wtime();

    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        int tid = omp_get_thread_num();
        B[i] = A[i] + constant;

        #pragma omp critical
        {
            fprintf(fp, "Index %d -> %d (Thread %d)\n", i, B[i], tid);
        }
    }

    end = omp_get_wtime();

    fclose(fp);

    // Display first 10 results
    printf("\nFirst 10 Results:\n");
    for (i = 0; i < n && i < 10; i++) {
        printf("A[%d]=%d + %d => B[%d]=%d\n",
               i, A[i], constant, i, B[i]);
    }

    printf("\nExecution Time = %f seconds\n", end - start);

    free(A);
    free(B);

    return 0;
}