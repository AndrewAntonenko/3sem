#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include <cstring>

#define MatrixSize 20
typedef double MatrixElem_t;
const int nThreads = 5;
struct matrix{
    int count;
    MatrixElem_t A[MatrixSize][MatrixSize] ;
    MatrixElem_t B[MatrixSize][MatrixSize] ;
};
matrix ArrMatrix [nThreads] = {};
MatrixElem_t C[MatrixSize][MatrixSize] ;
MatrixElem_t D[MatrixSize][MatrixSize] ;
int     i = 0,
        j = 0,
        k = 0;

pthread_t thid[nThreads];
inline void freeptr(void** ptr){
    free(*ptr);
    *ptr = NULL;
}
 void NewMatrix(){
    int     i = 0,
            j = 0;
    srand((unsigned int) time(NULL));
    for (i = 0; i < MatrixSize; i++){
        for ( j = 0; j < MatrixSize; j++) {
            ArrMatrix[0].A[i][j] = rand() % 100;
            ArrMatrix[0].B[i][j] = rand() % 100;
        }
    }
     for (int l = 1; l < nThreads; ++l) {
         memcpy(ArrMatrix[l].A, ArrMatrix[0].A, sizeof(MatrixElem_t) * MatrixSize * MatrixSize );
         memcpy(ArrMatrix[l].B, ArrMatrix[0].B, sizeof(MatrixElem_t) * MatrixSize * MatrixSize );
     }
    printf("matrix A\n");
     for (i = 0; i < MatrixSize; i++)
    {
        for (j = 0; j < MatrixSize; j++)
            printf("%lg ", ArrMatrix[0].A[i][j]);
        printf("\n");
    }
    printf("\nmatrix B\n");
    for (i = 0; i < MatrixSize; i++)
    {
        for (j = 0; j < MatrixSize; j++)
            printf("%lg ", ArrMatrix[0].B[i][j]);
        printf("\n");
    }
}
void print(int type){
    if (type){
        printf("\nthe result of multiplying with threads \n");
        for (i = 0; i < MatrixSize; i++)
        {
            for (j = 0; j < MatrixSize; j++)
                printf("%lg ", C[i][j]);
            printf("\n");
        }
    }
    else{
        printf("\nthe result of usual multiplying \n");
        for (i = 0; i < MatrixSize; i++)
        {
            for (j = 0; j < MatrixSize; j++)
                printf("%lg ", D[i][j]);
            printf("\n");
        }
    }
}
void* MultiplyThreads(void* counter ) {
    int iteration = ArrMatrix[*(int*)counter].count;
    for(i = iteration * MatrixSize / nThreads; i < (iteration + 1) * MatrixSize / nThreads ; i++)
        for(j = 0; j < MatrixSize; j++)
        {
            C[i][j] = 0;
            for(k = 0; k < MatrixSize; k++)
                C[i][j] += ArrMatrix[iteration].A[i][k] * ArrMatrix[iteration].B[k][j];
        }
    return NULL;
}
void* Multiply(void* ) {
    int     i = 0,
            j = 0,
            k = 0;
    for(i = 0; i < MatrixSize; i++)
        for(j = 0; j < MatrixSize; j++)
        {
            D[i][j] = 0;
            for(k = 0; k < MatrixSize; k++)
                D[i][j] += ArrMatrix[0].A[i][k] * ArrMatrix[0].B[k][j];
        }
    return NULL;
}

int main(){
    NewMatrix();
    clock_t start_time1, start_time2;
    clock_t search_time1, search_time2;
    start_time1 = clock();
    for (int i = 0; i < nThreads; ++i) {
        ArrMatrix[i].count = i;
        if(pthread_create(&thid[i], NULL, MultiplyThreads, (void*) &ArrMatrix[i].count )){
            perror("Error of creating new thread");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < nThreads; ++i) {
        pthread_join(thid[i], NULL);
    }
    search_time1 = clock() - start_time1;
    printf("Time of multiplying with threads %lu\n", search_time1 );
    start_time2 = clock();
    Multiply(NULL);
    search_time2 = clock() - start_time2;
    printf("Time of usual multiplying %lu\n", search_time2 );
    print(0);
    print(1);
    return 0;
}


