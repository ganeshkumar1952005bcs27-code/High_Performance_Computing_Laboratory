#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define N 20 

int main(int argc, char** argv) {
    int rank, size;
    int search_num, local_count = 0, total_count = 0;
    int *arr = NULL;
    int local_arr_size;

    double start_time, end_time, elapsed_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_arr_size = N / size;
    int *sub_arr = (int*)malloc(local_arr_size * sizeof(int));

    if (rank == 0) {
        arr = (int*)malloc(N * sizeof(int));
        srand(time(NULL));

        printf("Enter the number to find frequency: ");
        fflush(stdout);
        if (scanf("%d", &search_num) != 1) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Master process generating array...\n");
        for (int i = 0; i < N; i++) {
            arr[i] = rand() % 10;
            printf("%d ",arr[i]);
        }
        printf("\n");
    }

    // Distribute data
    MPI_Bcast(&search_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(arr, local_arr_size, MPI_INT, sub_arr, local_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
    start_time=MPI_Wtime();
    
    for (int i = 0; i < local_arr_size; i++) {
        if (sub_arr[i] == search_num) {
            local_count++;
        }
    }

    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;

    printf("Process %d: Counted %d. Execution time: %f seconds\n", rank, local_count, elapsed_time);

    if (rank == 0) {
        printf("\n--- Result ---\n");
        printf("Total Frequency of %d = %d\n", search_num, total_count);
        free(arr);
    }

    free(sub_arr);
    MPI_Finalize();
    return 0;
}
