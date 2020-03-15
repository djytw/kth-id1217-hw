#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_WORKERS 4
#define MAX_BODIES 240
#define DEFAULT_STEPS 20000
#define DEFAULT_DT 1
#define BODY_DIAMETER 0.1 //if distance is less than diameter, use diameter instead. prevent divide 0 error. also prevent huge force.
typedef struct{
    double x;
    double y;
}point;

//position, velocity, force and mass for each body
point p[240], v[240], f[MAX_WORKERS][240]; 
double m[240];
const double G = 6.67e-11;
int gnumBodies, numSteps, numWorkers;
double DT;
pthread_mutex_t barrier;
pthread_cond_t go;
int numArrived = 0;
long sliceLength = 0;
void *Worker(void*);

// calculate total force for ith body
void calculateForces(long myid) {
    double distance, magnitude; 
    point direction;
    //int cyc = numWorkers * 2;  //Use reversed strips, a cycle is 2*numworkers. 4workers cycle: W1 W2 W3 W4 W4 W3 W2 W1
    int i, j;
    for (i = myid; i < gnumBodies; i += numWorkers){
        for (j = i+1; j < gnumBodies; j++) {
            distance = sqrt( pow((p[i].x - p[j].x), 2) + pow((p[i].y - p[j].y), 2) );
            if (distance < BODY_DIAMETER) distance = BODY_DIAMETER;
            magnitude = (G * m[i] * m[j]) / pow(distance, 2);
            direction.x = p[j].x-p[i].x;
            direction.y = p[j].y-p[i].y;
            f[myid][i].x = f[myid][i].x + magnitude*direction.x/distance;
            f[myid][j].x = f[myid][j].x - magnitude*direction.x/distance;
            f[myid][i].y = f[myid][i].y + magnitude*direction.y/distance;
            f[myid][j].y = f[myid][j].y - magnitude*direction.y/distance;
            //printf("%ld ",myid);
        }
    }
}

// calculate new velocity and position for ith body
void moveBodies(long myid) {
    point deltav; // dv=f/m * DT
    point deltap; // dp=(v+dv/2) * DT
    point force;
    int i, k;
    for (i = myid*sliceLength; i < myid*sliceLength+sliceLength; i ++){
        force.x = 0;force.y = 0;
        for (k = 0; k < numWorkers; k ++){
            force.x += f[k][i].x;
            force.y += f[k][i].y;
            f[k][i].x = f[k][i].y = 0;
        }
        deltav.x = force.x/m[i] * DT;
        deltav.y = force.y/m[i] * DT;
        deltap.x = (v[i].x + deltav.x/2) * DT;
        deltap.y = (v[i].y + deltav.y/2) * DT;
        v[i].x = v[i].x + deltav.x;
        v[i].y = v[i].y + deltav.y;
        p[i].x = p[i].x + deltap.x;
        p[i].y = p[i].y + deltap.y;
    }
}

void initBodies(){
    //use same mass for all bodies, and place them on grid, so that it's easy to tell the program is correct
    int i;
    for (i = 1; i*i < gnumBodies; i++);
    int length = i;
    for (i = 0; i < gnumBodies; i++){
        p[i].x = i % length; //place on grid
        p[i].y = i / length;
        m[i] = 10000; //G is small, use big mass
        v[i].x = 0; // initial volocity is 0
        v[i].y = 0;
    }
}

void printBodies(const char* fileName){
    int i;
    FILE* fout = fopen(fileName, "w+");
    if (fout == NULL) return;

    for (i = 0; i < gnumBodies; i ++){
        fprintf(fout,"%.3g %.3g\n", p[i].x, p[i].y);
    }
    fclose(fout);
}

int main(int argc, char *argv[]) {

    if (argc <= 2){
        printf("Usage: %s gnumBodies numWorkers <numSteps> <DT>\n", argv[0]);
        return 0;
    }

    gnumBodies = (argc > 1)? atoi(argv[1]) : MAX_BODIES;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAX_WORKERS;
    numSteps = (argc > 3)? atoi(argv[3]) : DEFAULT_STEPS;
    DT = DEFAULT_DT;
    if (argc > 4) sscanf(argv[4], "%lf", &DT);
    if (gnumBodies > MAX_BODIES) gnumBodies = MAX_BODIES;
    if (numWorkers > MAX_WORKERS) numWorkers = MAX_WORKERS;
    if (gnumBodies%(numWorkers*2)) {
        printf("gnumBodies must divisble by numWorkers*2\n");
        return 0;
    }
    printf("parallel N^2 n-body. \ngnumBodies=%d, numSteps=%d, numWorkers=%d, DT=%lf\n", gnumBodies, numSteps, numWorkers, DT);
    sliceLength = gnumBodies/numWorkers;
    initBodies();

    struct timeval start, end;
    long l;
    pthread_t workerid[MAX_WORKERS];
    pthread_mutex_init(&barrier, NULL);
    pthread_cond_init(&go, NULL);
    printBodies("2-0.dat");

    gettimeofday( &start, NULL );
    
    for (l = 0; l < numWorkers; l++)
        pthread_create(&workerid[l], NULL, Worker, (void*)l);

    for (l = 0; l < numWorkers; l++){
        pthread_join(workerid[l], NULL);
    }

    gettimeofday( &end, NULL );
    printBodies("2-1.dat");
    double timeuse  = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("Finished, Total time = %.3f ms\n", timeuse/1000);
}
/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

void *Worker(void *arg) {
    long myid = (long) arg;
    int iter;
    for (iter = 0; iter < numSteps; iter ++){
        calculateForces(myid);
        Barrier();
        moveBodies(myid);
        Barrier();
    }
    return 0;
}