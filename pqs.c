#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define TASK_SIZE 100


int partition(int * a, int p, int r)
{
    int lt[r-p];
    int gt[r-p];
    int i;
    int j;
    int key = a[r];
    int lt_n = 0;
    int gt_n = 0;

    for(i = p; i < r; i++){
        if(a[i] < a[r]){
            lt[lt_n++] = a[i];
        }else{
            gt[gt_n++] = a[i];
        }   
    }   

    for(i = 0; i < lt_n; i++){
        a[p + i] = lt[i];
    }   

    a[p + lt_n] = key;

    for(j = 0; j < gt_n; j++){
        a[p + lt_n + j + 1] = gt[j];
    }   

    return p + lt_n;
}

void seq_quicksort(int arr[], int low, int high)
{
	int j;

	if(low < high){
		j = partition(arr, low, high);

		seq_quicksort(arr, low, j - 1);
		seq_quicksort(arr, j + 1, high);

	}
}


void quicksort(int * a, int p, int r)
{
    int div;

    if(p < r){ 
        div = partition(a, p, r); 
        #pragma omp task shared(a) if(r - p > TASK_SIZE) 
        quicksort(a, p, div - 1); 
        #pragma omp task shared(a) if(r - p > TASK_SIZE)
        quicksort(a, div + 1, r); 
    }
}

int main(int argc, char *argv[])
{	
	int n  = (argc > 1) ? atoi(argv[1]) : 200;
	int arr[n];
	int arr1[n];
        for(int i=0;i<n;i++){
		arr[i]=rand()%n;
		arr1[i]=arr[i];
	}
        int numThreads = 2;
	
        omp_set_dynamic(0);
        omp_set_num_threads(numThreads);
	double begin,end;
        begin = omp_get_wtime();
        seq_quicksort(arr,0,n-1);
        end = omp_get_wtime();
        printf("Sequential Time: %f (s) \n",end-begin);
        begin = omp_get_wtime();
        #pragma omp parallel
        {
            #pragma omp single
             quicksort(arr1, 0, n-1);
        }   
        end = omp_get_wtime();
        printf("Parallel Time: %f (s) \n",end-begin);
    return 0;
}
