#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void count_sort(int* arr, int n, int exp) { 
    int output[n]; 
    int i, count[10] = {0}; 
  
    for (i = 0; i < n; i++) 
        count[(arr[i] / exp) % 10]++; 
  
    for (i = 1; i < 10; i++) 
        count[i] += count[i - 1]; 
  
    for (i = n - 1; i >= 0; i--) 
    { 
        output[count[(arr[i] / exp) % 10] - 1] = arr[i]; 
        count[(arr[i] / exp) % 10]--; 
    }
  
    for (i = 0; i < n; i++) 
        arr[i] = output[i]; 
} 
  
void radix_sort(int* arr, int n) { 
    int m = get_max(arr, n); 
    for (int exp = 1; m/exp > 0; exp *= 10) {
        count_sort(arr, n, exp); 
    }
}

void print(int* arr, int n) { 
    for (int i = 0; i < n; i++) 
        printf("%d ", arr[i]); 
    printf("\n");
} 
  
int main(int argc, char *argv[]) { 
    if (argc != 2) {
        printf("Usage: ./serial_radix_sort <n>\n");
        return 1;
    }

    int n = atoi(argv[1]);
    int* arr = (int*) malloc(sizeof(int) * n);
    clock_t start, end;
    double cpu_time_used;

    rng(arr, n);
    printf("[Original array]\n");
    print(arr, n);

    start = clock();
    radix_sort(arr, n);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("[Sorted array]\n");
    print(arr, n);
    printf("[Sorted in %f seconds]\n", cpu_time_used);

    return 0; 
} 