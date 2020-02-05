#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

#define f(x) (sqrt(1-(x)*(x)))
#define MAXSLICE 1000000
#define MAXWORKERS 24

int NextPos = 0;
int Slices = MAXSLICE;
double Step = 0;
double Result;
long long Counter = 0;

pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

int NumWorkers;
double start_time, end_time;

double read_timer();
void *Worker(void*);

int main(int argc, char **argv){

    Slices = (argc > 1)? atoi(argv[1]) : MAXSLICE;
    Step = 1.0/(double)Slices;
    NumWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (NumWorkers > MAXWORKERS) NumWorkers = MAXWORKERS;

    long l;
    pthread_t workerid[MAXWORKERS];
    NextPos = 0;
    start_time = read_timer();
    
    for (l = 0; l < NumWorkers; l++)
        pthread_create(&workerid[l], NULL, Worker, (void*)l);

    for (l = 0; l < NumWorkers; l++){
        pthread_join(workerid[l], NULL);
    }
    
    end_time = read_timer();
    printf("pi = %.9lf count = %lld\n", Result * 4, Counter);
    printf("The execution time is %g sec\n", end_time - start_time);
    return 0;
}
double area(double x){
    __sync_synchronize();
    return (f(x) + f(x + Step)) * Step / 2;
}
void *Worker(void *arg) {
    int pos;
    volatile double t = 0;
    while(1){
        pthread_mutex_lock(&count_lock);
        pos = NextPos++;
        pthread_mutex_unlock(&count_lock);
        if (pos >= Slices)break;
        t += area((double)pos * Step);
    }
    Result += t;
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