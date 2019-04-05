#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

void rng(int* arr, int n) 
{
    int seed = 13516095;
    srand(seed);
    for(long i = 0; i < n; i++) {
        arr[i] = (int)rand();
    }
}

int main(int argc, char *argv[]) 
{
    int num_process, rank;
    MPI_Status Stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Generate random numbers
    int n = 10;
    int* numbers = (int*) malloc(sizeof(int) * n);
    rng(numbers, n);

    if (rank == 0) {
        printf("[MAIN]\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");
    }

    // Do count sort for each digit index
    int divisor = 1;

    // Scatter random numbers to all processes
    int n_per_proc = n / num_process;
    int* sub_numbers = (int*) malloc(sizeof(int) * n_per_proc);
    MPI_Scatter(numbers, n_per_proc, MPI_INT, sub_numbers, n_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

    int i, sub_count[10] = {0};
    for (i = 0; i < n_per_proc; i++) {
        sub_count[(sub_numbers[i] / divisor) % 10]++;
    }
    for (i = 1; i < 10; i++) {
        sub_count[i] += sub_count[i - 1];
    }

    printf("[SUB]\n");
    for (int i = 0; i < n_per_proc; i++) {
        printf("%d ", sub_numbers[i]);
    }
    printf("\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", sub_count[i]);
    }
    printf("\n");
    
    MPI_Finalize();
} 
