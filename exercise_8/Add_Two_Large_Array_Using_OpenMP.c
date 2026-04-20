#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main() {
    int n, i;
    double start, end;

    printf("Enter the size of arrays: ");
    scanf("%d", &n);

    int *A = (int *)malloc(n * sizeof(int));
    int *B = (int *)malloc(n * sizeof(int));
    int *C = (int *)malloc(n * sizeof(int));

    srand(time(0));

    // Generate random values (0–99)
    for (i = 0; i < n; i++) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    FILE *fp = fopen("output.txt", "w");

    start = omp_get_wtime();

    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        int tid = omp_get_thread_num();
        C[i] = A[i] + B[i];

        #pragma omp critical
        {
            fprintf(fp, "Index %d -> %d (Thread %d)\n", i, C[i], tid);
        }
    }

    end = omp_get_wtime();

    fclose(fp);

    printf("\nFirst 10 Results (A + B):\n");
    for (i = 0; i < n && i < 10; i++) {
        printf("A[%d]=%d + B[%d]=%d => C[%d]=%d\n",
               i, A[i], i, B[i], i, C[i]);
    }

    printf("\nExecution Time = %f seconds\n", end - start);

    free(A);
    free(B);
    free(C);

    return 0;
}