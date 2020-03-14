#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAX_BODIES 240
#define DEFAULT_STEPS 20000
#define DEFAULT_DT 0.01
#define BODY_DIAMETER 0.1 //if distance is less than diameter, use diameter instead. prevent divide 0 error. also prevent huge force.
typedef struct{
    double x;
    double y;
}point;

//position, velocity, force and mass for each body
point p[240], v[240], f[240]; 
double m[240];
const double G = 6.67e-11;
int gnumBodies, numSteps;
double DT;

// calculate total force for every pair of bodies
void calculateForces() {
    double distance, magnitude; 
    point direction;
    int i, j;
    for (i = 0; i < gnumBodies - 1; i ++){
        for (j = i+1; j < gnumBodies; j++) {
            distance = sqrt( pow((p[i].x - p[j].x), 2) + pow((p[i].y - p[j].y), 2) );
            if (distance < BODY_DIAMETER) distance = BODY_DIAMETER;
            magnitude = (G * m[i] * m[j]) / pow(distance, 2);
            direction.x = p[j].x-p[i].x;
            direction.y = p[j].y-p[i].y;
            f[i].x = f[i].x + magnitude*direction.x/distance;
            f[j].x = f[j].x - magnitude*direction.x/distance;
            f[i].y = f[i].y + magnitude*direction.y/distance;
            f[j].y = f[j].y - magnitude*direction.y/distance;
        }
    }
}

// calculate new velocity and position for each body
void moveBodies() {
    point deltav; // dv=f/m * DT
    point deltap; // dp=(v+dv/2) * DT
    int i;
    for (i = 0; i < gnumBodies; i++) {
        deltav.x = f[i].x/m[i] * DT;
        deltav.y = f[i].y/m[i] * DT;
        deltap.x = (v[i].x + deltav.x/2) * DT;
        deltap.y = (v[i].y + deltav.y/2) * DT;
        v[i].x = v[i].x + deltav.x;
        v[i].y = v[i].y + deltav.y;
        p[i].x = p[i].x + deltap.x;
        p[i].y = p[i].y + deltap.y;
        f[i].x = f[i].y = 0.0; // reset force vector
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
        f[i].x = 0;
        f[i].y = 0;
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

    if (argc <= 1){
        printf("Usage: %s gnumBodies <numSteps> <DT>\n", argv[0]);
        return 0;
    }

    gnumBodies = (argc > 1)? atoi(argv[1]) : MAX_BODIES;
    numSteps = (argc > 2)? atoi(argv[2]) : DEFAULT_STEPS;
    DT = DEFAULT_DT;
    if (argc > 3) sscanf(argv[3], "%lf", &DT);
    if (gnumBodies > MAX_BODIES) gnumBodies = MAX_BODIES;

    printf("sequential N^2 n-body. \ngnumBodies=%d, numSteps=%d, DT=%lf\n", gnumBodies, numSteps, DT);

    initBodies();

    struct timeval start, end;
    int i;
    gettimeofday( &start, NULL );
    for (i = 0; i < numSteps; i ++){
        calculateForces();
        moveBodies();
    }
    gettimeofday( &end, NULL );
    printBodies("1-1.dat");
    double timeuse  = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("Finished, Total time = %.3f ms\n", timeuse/1000);
}