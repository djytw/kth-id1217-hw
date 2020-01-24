#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define f(x) (sqrt(1-(x)*(x)))
#define MAXSLICE 1000000
#define MAXWORKERS 24

int NextPos = 0;
int Slices = MAXSLICE;
double Step = 0;
double Result = 0.0;
int NumWorkers;
double start_time, end_time;

double read_timer();
void *Worker(void*);

int main(int argc, char **argv){

    Slices = (argc > 1)? atoi(argv[1]) : MAXSLICE;
    Step = 1.0/(double)Slices;
    NumWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (NumWorkers > MAXWORKERS) NumWorkers = MAXWORKERS;

    int l;
    pthread_t workerid[MAXWORKERS];
    NextPos = 0;
    
    start_time = read_timer();
    
    for (l = 0; l < NumWorkers; l++)
        pthread_create(&workerid[l], NULL, Worker, NULL);

    for (l = 0; l < NumWorkers; l++)
        pthread_join(workerid[l], NULL);

    
    end_time = read_timer();
    printf("pi = %.15lf\n", Result * 4);
    printf("The execution time is %g sec\n", end_time - start_time);
    return 0;
}

void *Worker(void *arg) {
    int pos;
    double x,res;
    while(1){
        pos = __sync_fetch_and_add(&NextPos, 1);
        if (pos >= Slices)break;
        x = (double)pos * Step;
        res = (f(x) + f(x + Step)) * Step / 2;
        Result += res;
        //printf("%.15lf,%.15lf\n",x,res);
    }
    return NULL;
}

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