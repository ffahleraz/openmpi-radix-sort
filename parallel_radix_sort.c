#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

void rng(int* arr, int n) {
    int seed = 13516095;
    srand(seed);
    for(long i = 0; i < n; i++) {
        arr[i] = (int)rand();
    }
}

int get_max(int* arr, int n) { 
    int mx = arr[0]; 
    for (int i = 1; i < n; i++) 
        if (arr[i] > mx) 
            mx = arr[i]; 
    return mx; 
} 

int main(int argc, char *argv[]) { 
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

    // Compute sub count in each processes
    int i, sub_count[10] = {0};
    for (i = 0; i < n_per_proc; i++) {
        sub_count[(sub_numbers[i] / divisor) % 10]++;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // Reduce all the sub counts to root process
    if (rank == 0) {
        int count[10] = {0};
        MPI_Reduce(sub_count, count, 10, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        for (i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }
        
        int* temp_numbers = (int*) malloc(sizeof(int) * n);
        for (i = n - 1; i >= 0; i--) { 
            temp_numbers[count[(numbers[i] / divisor) % 10] - 1] = numbers[i]; 
            count[(numbers[i] / divisor) % 10]--; 
        }

        memcpy(numbers, temp_numbers, sizeof(int) * n);

        printf("[SORTED]\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

    } else {
        MPI_Reduce(sub_count, 0, 10, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
} 

// Driver program
// int main(int argc, char *argv[]) { 
//     int n = 10;
//     int arr[n];
//     rng(arr, n);
//     print(arr, n);
//     radix_sort(arr, n);
//     print(arr, n);
//     return 0; 
// } 