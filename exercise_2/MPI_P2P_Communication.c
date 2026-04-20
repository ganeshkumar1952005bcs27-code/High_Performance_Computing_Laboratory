#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mpi.h"

int isPalindrome(char str[]) {
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j) {
        if (str[i]!=str[j])
           return 0;
        i++;
        j--;
    }
    return 1;
}

int main()
{
    int rank, size;
    MPI_Status status;
    char message[100];
    char *str1 = "Hello1";
    char *str2 = "malayalam";
    char *str3 = "Welcome";
   
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(message, 100, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 1)
            {
                for (int j = 0; message[j]; j++) message[j] = toupper(message[j]);
                printf("Master: Rank %d sent Uppercase: %s\n", status.MPI_SOURCE, message);
            }
            else if (status.MPI_TAG == 2)
            {
                if (isPalindrome(message))
                    printf("Master: Rank %d sent %s is a Palindrome\n", status.MPI_SOURCE, message);
                else
                    printf("Master: Rank %d sent %s is NOT a Palindrome\n", status.MPI_SOURCE, message);
            }
        }
    }
    else
    {
        if (rank == 1)

            MPI_Send(str1, strlen(str1) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);

        else if (rank == 2)

            MPI_Send(str2, strlen(str2) + 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD);

        else if (rank == 3)

            MPI_Send(str3, strlen(str3) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}