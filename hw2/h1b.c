/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>
#include <stdlib.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
      //printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      	  //printf(" %d", matrix[i][j]);
	  }
	  //	  printf(" ]\n");
  }

  start_time = omp_get_wtime();


int Maxv = 0, Maxi = size, Maxj;
int Minv = __INT_MAX__, Mini = size, Minj;
#pragma omp parallel for reduction (+:total) private(j) 
  for (i = 0; i < size; i++){
    int maxv = 0, maxi = 0, maxj = 0;
    int minv = __INT_MAX__, mini = 0, minj = 0;
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if (matrix[i][j] > maxv){
          maxv = matrix[i][j];
          maxi = i;
          maxj = j;
      }
      if (matrix[i][j] < minv){
          minv = matrix[i][j];
          mini = i;
          minj = j;
      }
    }
    //printf("#%d:max=%d(%d,%d) min=%d(%d,%d)\n",omp_get_thread_num(),maxv,maxi,maxj,minv,mini,minj);
    #pragma omp critical
    if (maxv > Maxv){
      Maxv = maxv; Maxi = maxi; Maxj = maxj;
    }
    #pragma omp critical
    if (minv < Minv){
      Minv = minv; Mini = mini; Minj = minj;
    }
  }
// implicit barrier

  end_time = omp_get_wtime();
  printf("the total is %d\n", total);
  printf("max=%d(%d,%d) min=%d(%d,%d)\n",Maxv,Maxi,Maxj,Minv,Mini,Minj);
  printf("it took %g seconds\n", end_time - start_time);

}

