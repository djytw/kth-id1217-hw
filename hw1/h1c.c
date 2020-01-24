/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 24   /* maximum number of workers */

int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */

int matrix[MAXSIZE][MAXSIZE]; /* matrix */

void *Worker(void *);
typedef struct{
  int maxv,maxpi,maxpj,minv,minpi,minpj,sum;
}RESULT;

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  srand(time(NULL));
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
	  }
  }

  /* print the matrix */
  if (argc > 3 && *argv[3] == 'd')
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);

  RESULT final;
  final.maxv = -1;
  final.sum = 0;
  final.minv = __INT_MAX__;
  RESULT* temp;
  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l], (void**)&temp);
    
#ifdef DEBUG
  printf("worker %ld (pthread id %ld) has returned maxv=%d,maxpi=%d,maxpj=%d,minv=%d,minpi=%d,minpj=%d,sum=%d\n", l, workerid[l],temp->maxv,temp->maxpi,temp->maxpj,temp->minv,temp->minpi,temp->minpj,temp->sum);
#endif
    final.sum += temp->sum;
    if (temp->maxv > final.maxv){
      final.maxv = temp->maxv;
      final.maxpi = temp->maxpi;
      final.maxpj = temp->maxpj;
    }
    if (temp->minv < final.minv){
      final.minv = temp->minv;
      final.minpi = temp->minpi;
      final.minpj = temp->minpj;
    }
    free(temp);
  }
  
  /* get end time */
  end_time = read_timer();
  /* print results */
  printf("The total is %d\n", final.sum);
  printf("Max: %d at (%d,%d); Min: %d at (%d,%d)\n", final.maxv, final.maxpi, final.maxpj, final.minv, final.minpi, final.minpj);
  printf("The execution time is %g sec\n", end_time - start_time);
  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int total, i, j, first, last;

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

#ifdef DEBUG
  printf("worker %ld (pthread id %ld) has started first=%d last=%d\n", myid, pthread_self(),first,last);
#endif

  /* sum values in my strip */
  total = 0;
  int maxv = -1, maxpi, maxpj;
  int minv = __INT_MAX__, minpi, minpj; 

  for (i = first; i <= last; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if (matrix[i][j] > maxv){
        maxv = matrix[i][j];
        maxpi = i; maxpj = j;
      }
      if (matrix[i][j] < minv){
        minv = matrix[i][j];
        minpi = i; minpj = j;
      }
    }
  
  RESULT* ret = malloc(sizeof(RESULT));
  ret->maxpi = maxpi;
  ret->maxpj = maxpj;
  ret->maxv = maxv;
  ret->minpi = minpi;
  ret->minpj = minpj;
  ret->minv = minv;
  ret->sum = total;

#ifdef DEBUG
  printf("worker %ld (pthread id %ld) finished maxv=%d,maxpi=%d,maxpj=%d,minv=%d,minpi=%d,minpj=%d,sum=%d\n", myid, pthread_self(),ret->maxv,ret->maxpi,ret->maxpj,ret->minv,ret->minpi,ret->minpj,ret->sum);
#endif
  pthread_exit(ret);
}
