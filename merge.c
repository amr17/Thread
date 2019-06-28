#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//array size
int  A;

//2D Arrays
int *B;


struct  position
{
    int low;
    int high;
};

void read()
{
    FILE *inp = fopen("input.txt", "r");
     if (inp == NULL) {
        printf("File not found");
        exit(0);
    }

    //inputs for merge sort
    fscanf(inp, "%d", &A);
    printf("Number of elements to be sorted #%d\n", A);
    B=malloc(A * sizeof(int));
    for (int i = 0; i < A; i++ )
        fscanf(inp, "%d",&B[i]);

    fclose(inp);
    for (int i = 0; i <A ; ++i)printf("%d ",B[i]);
    printf("\n");
}

void merge(int lo, int me, int hi)
{
    int *left = malloc((me-lo+1)*sizeof(int));
    int *right = malloc((hi-me)*sizeof(int));

    // n1 is size of left part and n2 is size
    // of right part
    int n1 = me-lo+1, n2 = hi-me, i, j;

    // storing values in left part
    for (i = 0; i < n1; i++)
        left[i] = B[i + lo];

    // storing values in right part
    for (i = 0; i < n2; i++)
        right[i] = B[i+me+ 1];

    int k = lo;
    i = j = 0;

    // merge left and right in ascending order
    while (i < n1 && j < n2)
    {
        if (left[i] <= right[j])
            B[k++] = left[i++];
        else
            B[k++] = right[j++];
    }

    // insert remaining values from left
    while (i < n1)
    {
        B[k++] = left[i++];
    }

    // insert remaining values from right
    while (j < n2)
    {
        B[k++] = right[j++];
    }
}

void *mergesort(void *n)
{
    struct position *data = n;
    struct position n1, n2;
    pthread_t pthread, pthread1;
    int mid = (data->low+data->high)/2;
    n1.low=data->low;
    n1.high=mid;
    n2.low=mid +1;
    n2.high=data->high;
    if (data->low<data->high)
    {
        pthread_create(&pthread,NULL,mergesort,&n1);
        pthread_create(&pthread1,NULL,mergesort,&n2);
        pthread_join(pthread,NULL);
        pthread_join(pthread1,NULL);
        merge(data->low,mid,data->high);

    }
}

void merge_thread()
{
    read();
    clock_t begin = clock();
    pthread_t pthread;
    struct position *data = (struct position *) malloc(sizeof(struct position));
    data->low=0;
    data->high=A-1;
    pthread_create(&pthread,NULL,mergesort,data);
    pthread_join(pthread,NULL);
    printf("Sorted Array : ");
    for (int j = 0; j < A; ++j)
    {
        printf("%d ", B[j]);
    }
    printf("\n");
    clock_t end = clock();
    printf("Time taken:%lfsec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}


void main()
{

merge_thread();



}
