#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
//
// u8 [8] -> map 
// each char represent a row in the map
//  (little-endian)
//eg.
// ef 01 00 00 00 00 00 00 ->
// x x x x o x x x 
// x o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
//
//
// u8 [8] -> placement
// each char represent a chess position
//  (increasing sequence)
//eg.
// 00 01 02 03 05 06 07 08 ->
// x x x x o x x x 
// x o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
// o o o o o o o o
//

//utils
void place2map(uint8_t *place, uint8_t *map){
    int i;
    memset(map, 0, 8);
    for (i = 0; i < 8; i++){
        map[place[i] / 8] |= (1 << (place[i] % 8));
    }
}
void map2place(uint8_t *map, uint8_t *place){
    int i,j,top = 0;
    for (i = 0; i < 8; i++){
        if (map[i] == 0) continue;
        for (j = 0; j < 8; j++){
            if ((map[i] & (1 << j)) != 0){
                place[top++] = 8 * i + j;
            }
        }
    }
}
// if place[0] == 0x38(56) finished
void next_placement(uint8_t *place){
    int i;
    for (i = 7; i >= 0; i--){
        if (place[i] < 56 + i){
            place[i] ++;
            for (i++; i < 8; i++){
                place[i] = place[i-1] + 1;
            }
            return;
        }
    }
}

//main
#define STACK_LEN 10000
#define MAXWORKERS 24
int Stack_Top = 0;
uint8_t Stack[STACK_LEN*8]; //map
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
bool Finished = false;
int Solutions = 0;
int NumWorkers;
double start_time, end_time;
long long Counter = 0;

double read_timer();
void *Worker(void*);
int main(int argc, char **argv){

    NumWorkers = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
    if (NumWorkers > MAXWORKERS) NumWorkers = MAXWORKERS;
    long l;
    pthread_t workerid[MAXWORKERS];

    start_time = read_timer();
    for (l = 0; l < NumWorkers; l++)
        pthread_create(&workerid[l], NULL, Worker, (void*)l);
    
    printf("pthread create OK\n");
    uint8_t place[8] = {0,1,2,3,4,5,6,7};
    while(place[0] < 8){
        pthread_mutex_lock(&count_lock);
        if (Stack_Top >= STACK_LEN){
            pthread_mutex_unlock(&count_lock);
            usleep(1000);
            continue;
        }
        place2map(place, &Stack[Stack_Top*8]);
        Stack_Top++;
        pthread_mutex_unlock(&count_lock);
        next_placement(place);
    }
    Finished = true;
    printf("placement generate finished\n");

    for (l = 0; l < NumWorkers; l++){
        pthread_join(workerid[l], NULL);
    }
    
    end_time = read_timer();
    printf("Total solutions = %d\n", Solutions);
    printf("The execution time is %g sec\n", end_time - start_time);
    return 0;
}
void *Worker(void *arg) {
    uint8_t *map, place[8], maps[800];
    int head = 0;
    while(1){
        pthread_mutex_lock(&count_lock);
        if (Stack_Top == 0){
            if (Finished){
                pthread_mutex_unlock(&count_lock);
                pthread_exit(0);
            }
            pthread_mutex_unlock(&count_lock);
            usleep(1000);
            continue; 
        }
        if (Stack_Top > 100){
            Stack_Top -= 100;
            memcpy(maps, &Stack[Stack_Top*8], 800);
            head = 100;
            Counter+=100;
            if(Counter / 100 % 1000000 == 0){
                end_time = read_timer();
                printf("Worker Counter = %lldM, Completed=%2.2lf%% (Total~=4426M), run=%gs\n",Counter/1000000, (double)Counter/10000/4426, end_time - start_time);
            }
        }else{
        Counter ++;
            if (Counter % 100000000 == 0){
                end_time = read_timer();
                printf("Worker Counter = %lldM, Completed=%2.2lf%% (Total~=4426M), run=%gs\n",Counter/1000000, (double)Counter/10000/4426, end_time - start_time);
            }
            Stack_Top --;
            memcpy(maps, &Stack[Stack_Top*8], 8);
            head = 1;
        }
        pthread_mutex_unlock(&count_lock);

        while (head > 0){
        head --;
        map = &maps[head*8];
        //check row
        int i;
        for (i = 0; i < 8; i++){
            if (map[i] == 0){
                break;
            }
        }
        if (i != 8) continue;

        //check col
        uint8_t t = 0;
        for (i = 0; i < 8; i++){
            t |= map[i];
        }
        if (t != 0xff) continue;

        //check diagonal
        int j;
        map2place(map, place);
        for (i = 0; i < 7; i++){
            for (j = i + 1; j < 8; j++){
                if (((place[j]%8) < (place[i]%8)) &&((place[j] - place[i]) % 7) == 0){
                    break;
                }
                if (((place[j]%8) > (place[i]%8)) &&((place[j] - place[i]) % 9) == 0){
                    break;
                }
            }
            if (j != 8) break;
        }
        if (i != 7) continue;

        //check ok print
        pthread_mutex_lock(&count_lock);
        Solutions++;
        printf("Solution %d:\n", Solutions);
        for (i = 0; i < 8; i++){
            for (j = 0; j < 8; j++){
                if ((map[i] & (1 << j)) != 0){
                    printf("x ");
                }else{
                    printf("o ");
                }
            }
            printf("\n");
        }
        printf("\n");
        pthread_mutex_unlock(&count_lock);
    }}
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