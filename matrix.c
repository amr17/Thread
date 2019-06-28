#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// declaration of global matrices 

int **matrixA;
int aRows, aColumns;

int **matrixB;
int bRows, bColumns;

int **matrixC;
int cRows, cColumns;

struct ThreadData {
    int aRowIndex;
    int bColumnIndex;
};


  //calculates each element in output matrixC in a thread
 
void *calculateElement(void *args) {

 //struct that contains which row from matrixA to multiply and get sum with which column from matrixB

    struct ThreadData *data = (struct ThreadData *) args;

    int rowIndex = data->aRowIndex;
    int columnIndex = data->bColumnIndex;


    int sum = 0;

    for (int i = 0; i < aColumns; i++)
        sum += matrixA[rowIndex][i] * matrixB[i][columnIndex];

    matrixC[rowIndex][columnIndex] = sum;


    free(data);

}

void method1() {

    int rc;
    pthread_attr_t attr;
    pthread_t thread[aRows * bColumns];
    void *status;

    //Initialize and set thread detached attribute
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < aRows; i++) {
        for (int j = 0; j < bColumns; j++) {

            //struct to pass arguments to each thread
            struct ThreadData *threadData = malloc(sizeof(struct ThreadData));
            threadData->aRowIndex = i;
            threadData->bColumnIndex = j;

            //create thread
            rc = pthread_create(&thread[i * bColumns + j], &attr, calculateElement, (void *) threadData);

            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }

    }


    // Free attribute and wait for the other threads by using join
    pthread_attr_destroy(&attr);
    for (int i = 0; i < aRows * bColumns; i++) {
        rc = pthread_join(thread[i], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }


    }

}

// calculates each row of the output matrix c in a thread
void *calculateRow(void *args) {

//args takes matrixC row index to calculate
    int cRowToCalculate = (int) args;

    int sum;
    for (int i = 0; i < cColumns; i++) {
        sum = 0;
        for (int j = 0; j < aColumns; j++)
            sum += matrixA[cRowToCalculate][j] * matrixB[j][i];
        matrixC[cRowToCalculate][i] = sum;

    }

}

void method2() {

    int rc;
    pthread_attr_t attr;
    pthread_t thread[cRows];
    void *status;

    //Initialize and set thread detached attribute
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < cRows; i++) {

        //create thread
        rc = pthread_create(&thread[i], &attr, calculateRow, (void *) i);

        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

    }


    // Free attribute and wait for the other threads by using join
    pthread_attr_destroy(&attr);
    for (int i = 0; i < cRows; i++) {
        rc = pthread_join(thread[i], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
}


int main() {

    FILE *input = fopen("inputmul.txt", "r");

    if (input == NULL) {
        puts("couldn't find input file");
        exit(0);
    }

    //read matrixA then matrixB
    fscanf(input, "%d", &aRows);
    fscanf(input, "%d", &aColumns);

    matrixA = (int **) malloc(aRows * sizeof(int *));

    for (int i = 0; i < aRows; i++) {
        matrixA[i] = (int *) malloc(aColumns * sizeof(int));
        for (int j = 0; j < aColumns; j++)
            fscanf(input, "%d", &matrixA[i][j]);
    }


    fscanf(input, "%d", &bRows);
    fscanf(input, "%d", &bColumns);

    matrixB = (int **) malloc(bRows * sizeof(int *));

    for (int i = 0; i < bRows; i++) {
        matrixB[i] = (int *) malloc(bColumns * sizeof(int));
        for (int j = 0; j < bColumns; j++)
            fscanf(input, "%d", &matrixB[i][j]);
    }

    fclose(input);


    cRows = aRows;
    cColumns = bColumns;

    matrixC = (int **) malloc(cRows * sizeof(int *));

    for (int i = 0; i < aRows; i++) {
        matrixC[i] = (int *) malloc(cColumns * sizeof(int));
    }

    FILE *output = fopen("output.txt", "w");
    clock_t begin, end;

    begin = clock();
    method1();
    end = clock();

    for (int i = 0; i < cRows; i++) {
        for (int j = 0; j < cColumns; j++) {
            fprintf(output, "%d ", matrixC[i][j]);
        }
        fprintf(output, "\n");
    }

    fprintf(output, "END1 [%lf]\n", (double) (end - begin) / CLOCKS_PER_SEC);

    begin = clock();
    method2();
    end = clock();

    for (int i = 0; i < cRows; i++) {
        for (int j = 0; j < cColumns; j++) {
            fprintf(output, "%d ", matrixC[i][j]);
        }
        fprintf(output, "\n");
    }

    fprintf(output, "END2 [%lf]", (double) (end - begin) / CLOCKS_PER_SEC);


    fclose(output);

    return 0;
}

