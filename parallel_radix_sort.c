#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

void count_sort(int* arr, int n, int divisor, int num_process, int rank) {
    // Scatter random numbers to all processes
    int n_per_proc = n / num_process;
    int* sub_arr = (int*) malloc(sizeof(int) * n_per_proc);
    MPI_Scatter(arr, n_per_proc, MPI_INT, sub_arr, n_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute sub count in each processes
    int i, sub_count[10] = {0};
    for (i = 0; i < n_per_proc; i++) {
        sub_count[(sub_arr[i] / divisor) % 10]++;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // Reduce all the sub counts to root process
    if (rank == 0) {
        int count[10] = {0};
        MPI_Reduce(sub_count, count, 10, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        for (i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }
        
        int* temp_arr = (int*) malloc(sizeof(int) * n);
        for (i = n - 1; i >= 0; i--) { 
            temp_arr[count[(arr[i] / divisor) % 10] - 1] = arr[i]; 
            count[(arr[i] / divisor) % 10]--; 
        }
        memcpy(arr, temp_arr, sizeof(int) * n);
        free(temp_arr);

    } else {
        MPI_Reduce(sub_count, 0, 10, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    free(sub_arr);
}

void radix_sort(int* arr, int n, int num_process, int rank) { 
    int m = get_max(arr, n); 
    for (int divisor = 1; m / divisor > 0; divisor *= 10) {
        count_sort(arr, n, divisor, num_process, rank);
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void print(int* arr, int n) { 
    for (int i = 0; i < n; i++) 
        printf("%d ", arr[i]); 
    printf("\n");
} 

int main(int argc, char *argv[]) {
    // Init MPI communication
    int num_process, rank;
    MPI_Status Stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Sort
    int n = 10;
    int* arr = (int*) malloc(sizeof(int) * n);

    if (rank == 0) {
        rng(arr, n);
        printf("[Original array]\n");
        print(arr, n);

        clock_t start, end;
        double cpu_time_used;

        start = clock();
        radix_sort(arr, n, num_process, rank);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("[Sorted array]\n");
        printf("[Sorted in %f seconds]\n", cpu_time_used);
        print(arr, n);
    } else {
        radix_sort(arr, n, num_process, rank);
    }

    MPI_Finalize();
    return 0; 
} 