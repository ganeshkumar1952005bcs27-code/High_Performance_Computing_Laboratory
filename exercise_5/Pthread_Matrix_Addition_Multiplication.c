#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

double get_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void* matrixAddition(void* arg) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    void** args = (void**)arg;
    int** A = (int**)args[0];
    int** B = (int**)args[1];
    int** Res = (int**)args[2];
    int* dims = (int*)args[3];

    for(int i = 0; i < dims[0]; i++) {
        for(int j = 0; j < dims[1]; j++) {
            Res[i][j] = A[i][j] + B[i][j];
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("[Thread 1] Addition completed in: %.6f seconds\n", get_elapsed_time(start, end));
    pthread_exit(NULL);
}

void* matrixMultiplication(void* arg) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    void** args = (void**)arg;
    int** A = (int**)args[0];
    int** B = (int**)args[1];
    int** Res = (int**)args[2];
    int* dims = (int*)args[3];
    int r1 = dims[0], c1 = dims[1], c2 = dims[2];

    for(int i = 0; i < r1; i++) {
        for(int j = 0; j < c2; j++) {
            Res[i][j] = 0;
            for(int k = 0; k < c1; k++) {
                Res[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("[Thread 2] Multiplication completed in: %.6f seconds\n", get_elapsed_time(start, end));
    pthread_exit(NULL);
}

int** allocateMatrix(int r, int c) {
    int** mat = (int**)malloc(r * sizeof(int*));
    for (int i = 0; i < r; i++) mat[i] = (int*)malloc(c * sizeof(int));
    return mat;
}

void freeMatrix(int** mat, int r) {
    for (int i = 0; i < r; i++) free(mat[i]);
    free(mat);
}

void printMatrix(const char* name, int** mat, int r, int c) {
    printf("\n--- %s Result (%dx%d) ---\n", name, r, c);
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++) printf("%d ", mat[i][j]);
        printf("\n");
    }
}

int main() {
    pthread_t t1, t3;
    int r1, c1, r2, c2;

    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &r1, &c1);
    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &r2, &c2);

    int** A = allocateMatrix(r1, c1);
    int** B = allocateMatrix(r2, c2);

    printf("Enter elements of Matrix A:\n");
    for(int i = 0; i < r1; i++) for(int j = 0; j < c1; j++) scanf("%d", &A[i][j]);
    printf("Enter elements of Matrix B:\n");
    for(int i = 0; i < r2; i++) for(int j = 0; j < c2; j++) scanf("%d", &B[i][j]);

    int** Add = (r1 == r2 && c1 == c2) ? allocateMatrix(r1, c1) : NULL;
    int addSubDims[2] = {r1, c1};

    int** Mul = (c1 == r2) ? allocateMatrix(r1, c2) : NULL;
    int mulDims[3] = {r1, c1, c2};

    void* addArgs[4] = {A, B, Add, addSubDims};
    void* mulArgs[4] = {A, B, Mul, mulDims};

    printf("\n--- Starting Parallel Computation ---\n");

    if (Add) {
        pthread_create(&t1, NULL, matrixAddition, addArgs);
    }
    if (Mul) pthread_create(&t3, NULL, matrixMultiplication, mulArgs);

    // Wait for all threads to finish
    if (Add) pthread_join(t1, NULL);
    if (Mul) pthread_join(t3, NULL);

    if (Add) {
        printMatrix("Addition", Add, r1, c1);
    }

    if (Mul) {
        printMatrix("Multiplication", Mul, r1, c2);
    } else {
        printf("\nMultiplication not possible (columns of A must equal rows of B).\n");
    }

    freeMatrix(A, r1); freeMatrix(B, r2);
    if (Add) { freeMatrix(Add, r1);  }
    if (Mul) freeMatrix(Mul, r1);

    return 0;
}