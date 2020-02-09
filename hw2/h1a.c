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


int maxv[MAXWORKERS], maxi[MAXWORKERS], maxj[MAXWORKERS];
int minv[MAXWORKERS], mini[MAXWORKERS], minj[MAXWORKERS];
for(i = 0; i < numWorkers; i++){maxv[i] = 0; minv[i] = __INT_MAX__;}
#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      int myid = omp_get_thread_num();
      total += matrix[i][j];
      if (matrix[i][j] > maxv[myid]){
          maxv[myid] = matrix[i][j];
          maxi[myid] = i;
          maxj[myid] = j;
      }
      if (matrix[i][j] < minv[myid]){
          minv[myid] = matrix[i][j];
          mini[myid] = i;
          minj[myid] = j;
      }
    }
// implicit barrier

  end_time = omp_get_wtime();
for(i = 0; i < numWorkers; i++){
    printf("Worker #%d: max=%d(%d,%d) min=%d(%d,%d)\n",i,maxv[i],maxi[i],maxj[i],minv[i],mini[i],minj[i]);
    if(maxv[i]>maxv[0]){
        maxv[0]=maxv[i];
        maxi[0]=maxi[i];
        maxj[0]=maxj[i];
    }
    if(minv[i]<minv[0]){
        minv[0]=minv[i];
        mini[0]=mini[i];
        minj[0]=minj[i];
    }
}
  printf("the total is %d\n", total);
  printf("max=%d(%d,%d) min=%d(%d,%d)\n",maxv[0],maxi[0],maxj[0],minv[0],mini[0],minj[0]);
  printf("it took %g seconds\n", end_time - start_time);

}

