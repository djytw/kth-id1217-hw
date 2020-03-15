#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAX_BODIES 240
#define DEFAULT_STEPS 20000
#define DEFAULT_DT 1
#define DEFAULT_LIMIT 40
#define DEFAULT_THETA 0.5
#define BODY_DIAMETER 0.1 //if distance is less than diameter, use diameter instead. prevent divide 0 error. also prevent huge force.

typedef struct{
    double x;
    double y;
}point;

typedef struct Quad{
    int index; //if it's leaf, this is the body index. -2 if it's empty, otherwise it's -1
    double mass; //total mass
    double x,y; // center of mass
    struct Quad* sub[4]; 
    double tx,ty; //center of rect 
    double size; //half-length
    //  0 | 1
    // ---+---
    //  2 | 3
}Quad;

//position, velocity, force and mass for each body
point p[240], v[240], f[240]; 
double m[240];
const double G = 6.67e-11;
int gnumBodies, numSteps, xylimit;
double DT, theta;
Quad* Root;

void initBodies(){
    //use same mass for all bodies, and place them on grid, so that it's easy to tell the program is correct
    int i;
    for (i = 1; i*i < gnumBodies; i++);
    int length = i;
    for (i = 0; i < gnumBodies; i++){
        p[i].x = i % length - length/2; //place on grid
        p[i].y = i / length - length/2;
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

void constructQuadtree(){
    Root = (Quad*)malloc(sizeof(Quad));
    Root->index = 0;
    Root->tx = 0;
    Root->ty = 0;
    Root->size = xylimit;
    int i, j;
    for (i = 1; i < gnumBodies; i++){
        Quad* q = Root;
        //find node
        while(q->index == -1){
            if (p[i].x <= q->tx){
                if (p[i].y <= q->ty)q = q->sub[2];
                else q = q->sub[0];
            }else{
                if (p[i].y <= q->ty)q = q->sub[3];
                else q = q->sub[1];
            }
        }
        //if empty, set node info
        if (q->index == -2){
            q->index = i;
            continue;
        }
        //not empty, create subtree
        while(1){
            for(j = 0; j < 4; j++) {
                q->sub[j] = (Quad*)malloc(sizeof(Quad));
                q->sub[j]->index = -2;
                q->sub[j]->size =q->size/2;
            }
            q->sub[0]->tx = q->tx - q->size/2;
            q->sub[0]->ty = q->ty + q->size/2;
            q->sub[1]->tx = q->tx + q->size/2;
            q->sub[1]->ty = q->ty + q->size/2;
            q->sub[2]->tx = q->tx - q->size/2;
            q->sub[2]->ty = q->ty - q->size/2;
            q->sub[3]->tx = q->tx + q->size/2;
            q->sub[3]->ty = q->ty - q->size/2;
            int temp;
            if (p[q->index].x <= q->tx){
                if (p[q->index].y <= q->ty)temp = 2;
                else temp = 0;
            }else{
                if (p[q->index].y <= q->ty)temp = 3;
                else temp = 1;
            }
            q->sub[temp]->index = q->index;
            q->index = -1;
            int temp2;
            if (p[i].x <= q->tx){
                if (p[i].y <= q->ty)temp2 = 2;
                else temp2 = 0;
            }else{
                if (p[i].y <= q->ty)temp2 = 3;
                else temp2 = 1;
            }
            if (temp2 != temp) {
                q->sub[temp2]->index = i;
                break;
            }
            q = q->sub[temp];
        }
    }
}
// calculate mass and mass center
void fillQuadtree(Quad* q){
    if (q->index == -2) return;
    if (q->index != -1){
        q->mass = m[q->index];
        q->x = p[q->index].x;
        q->y = p[q->index].y;
    }else{
        fillQuadtree(q->sub[0]);
        fillQuadtree(q->sub[1]);
        fillQuadtree(q->sub[2]);
        fillQuadtree(q->sub[3]);
        q->mass = q->sub[0]->mass + q->sub[1]->mass + q->sub[2]->mass + q->sub[3]->mass;
        q->x = ( q->sub[0]->x * q->sub[0]->mass +
                 q->sub[1]->x * q->sub[1]->mass +
                 q->sub[2]->x * q->sub[2]->mass +
                 q->sub[3]->x * q->sub[3]->mass ) / q->mass;
        q->y = ( q->sub[0]->y * q->sub[0]->mass +
                 q->sub[1]->y * q->sub[1]->mass +
                 q->sub[2]->y * q->sub[2]->mass +
                 q->sub[3]->y * q->sub[3]->mass ) / q->mass;
    }
}

void calculateForce(int id, Quad* q){
    double distance, magnitude; 
    point direction;
    if (q->index == -2) return;
    distance = sqrt( pow((p[id].x - q->x), 2) + pow((p[id].y - q->y), 2) );
    if (q->index != -1 || theta > q->size/distance) {
        //q is a leaf calc directly, or far enough, use approximation
        if (distance < BODY_DIAMETER) distance = BODY_DIAMETER;
        magnitude = (G * m[id] * q->mass) / pow(distance, 2);
        direction.x = q->x-p[id].x;
        direction.y = q->y-p[id].y;
        f[id].x = f[id].x + magnitude*direction.x/distance;
        f[id].y = f[id].y + magnitude*direction.y/distance;
    }else{
        calculateForce(id, q->sub[0]);
        calculateForce(id, q->sub[1]);
        calculateForce(id, q->sub[2]);
        calculateForce(id, q->sub[3]);
    }
}

void deleteTree(Quad* q){
    if (q->index != -1){
        free(q);
    }else{
        deleteTree(q->sub[0]);
        deleteTree(q->sub[1]);
        deleteTree(q->sub[2]);
        deleteTree(q->sub[3]);
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
        if (xylimit){
            if(p[i].x > xylimit) p[i].x = xylimit;
            if(p[i].x < -xylimit) p[i].x = -xylimit;
            if(p[i].y > xylimit) p[i].y = xylimit;
            if(p[i].y < -xylimit) p[i].y = -xylimit;
        }
        f[i].x = f[i].y = 0.0; // reset force vector
    }
}

int main(int argc, char *argv[]) {

    if (argc <= 1){
        printf("Usage: %s gnumBodies <theta> <numSteps> <DT> <XY-Limit>\n", argv[0]);
        return 0;
    }

    gnumBodies = (argc > 1)? atoi(argv[1]) : MAX_BODIES;
    theta = DEFAULT_THETA;
    if (argc > 2) sscanf(argv[2], "%lf", &theta);
    theta/=2; //because quadtree->size is half of the length
    numSteps = (argc > 3)? atoi(argv[3]) : DEFAULT_STEPS;
    DT = DEFAULT_DT;
    if (argc > 4) sscanf(argv[4], "%lf", &DT);
    if (gnumBodies > MAX_BODIES) gnumBodies = MAX_BODIES;
    xylimit = (argc > 5)? atoi(argv[5]) : DEFAULT_LIMIT;

    printf("sequential Barnes-Hut n-body. \ngnumBodies=%d, theta=%lf, numSteps=%d, DT=%lf\n", gnumBodies, theta, numSteps, DT);

    initBodies();
    struct timeval start, end;
    int i, j;
    gettimeofday( &start, NULL );
    ////////////////////TIMING///////////////////////
    for (i = 0; i < numSteps; i ++){
        constructQuadtree();
        fillQuadtree(Root);
        for (j = 0; j < gnumBodies; j++){
            f[j].x = f[j].y = 0;
            calculateForce(j, Root);
        }
        deleteTree(Root);
        moveBodies();
    }
    ////////////////////ENDTIMING/////////////////////
    gettimeofday( &end, NULL );
    printBodies("3-1.dat");
    double timeuse  = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("Finished, Total time = %.3f ms\n", timeuse/1000);
}