#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <omp.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
int count=0;
 
void print(uint64_t q){
#pragma omp atomic    
    count++;
#ifdef OUTPUT    
#pragma omp critical
{
    uint8_t* queen = (uint8_t*)&q;
    int i, j;
    printf("Placement #%d:\n",count);
    for(i = 0; i < 8; i++){
        for (j = 0; j < queen[i]; j++){
            printf(". ");
        }
        printf("x ");
        for (; j < 8; j ++){
            printf(". ");
        }
        printf("\n");
    }
}
#endif
}
 
bool canPlaceQueen(uint64_t q,int k){
    uint8_t* queen = (uint8_t*)&q;
    for(int i = 0; i < k; i++){
        if(queen[i] == queen[k] || abs(k-i) == abs(queen[k]-queen[i])) return false;
    }
    return true;
}

void eightQueen(uint64_t q,int k){
    uint8_t* queen = (uint8_t*)&q;
    for(int i=0; i<8; i++){
        queen[k] = i;
        if(k == 7 && canPlaceQueen(q,k))
        {
            print(q);
            return;
        }
        else if(canPlaceQueen(q,k))
        {
            eightQueen(q,k+1);
        }
    }
}
#define MAXWORKERS 8
int main(int argc, char* argv[]){

    int i;
    int numWorkers = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    omp_set_num_threads(numWorkers);
    
    double start_time, end_time;
    start_time = omp_get_wtime();
    #pragma omp parallel for
    for(i = 0; i < 8; i++){
        uint64_t q = 0;
        uint8_t* queen = (uint8_t*)&q;
        queen[0] = i;
        eightQueen(q, 1);
    }
    end_time = omp_get_wtime();
    printf("Total = %d\n",count);
    printf("The execution time is %g sec\n", end_time - start_time);
    return 0;
}