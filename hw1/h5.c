#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_LEN 10000
#define MAX_WORKERS 24

void *Worker(void*);
bool Finished = false;
FILE *File1, *File2;
unsigned long Linelen = MAX_LINE_LEN;
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv){

    if (argc != 3 && argc != 4){
        printf("Usage: %s <filename1> <filename2> [threads]\n", argv[0]);
        exit(-1);
    }
    File1 = fopen(argv[1], "r");
    if (File1 == NULL){
        printf("File1 open failed. %s\n", strerror(errno));
        exit(-1);
    }
    File2 = fopen(argv[2], "r");
    if (File2 == NULL){
        printf("File2 open failed. %s\n", strerror(errno));
        exit(-1);
    }
    int numWorkers;
    numWorkers = (argc == 4)? atoi(argv[3]) : MAX_WORKERS;
    if (numWorkers > MAX_WORKERS || numWorkers < 1) numWorkers = MAX_WORKERS;

    int l;
    pthread_t workerid[MAX_WORKERS];
    for (l = 0; l < numWorkers; l++)
        pthread_create(&workerid[l], NULL, Worker, NULL);
    pthread_exit(NULL);
}

void *Worker(void *arg) {
    int size1, size2;
    char *line1, *line2;
    line1 = (char*) malloc(sizeof(char) * MAX_LINE_LEN);
    line2 = (char*) malloc(sizeof(char) * MAX_LINE_LEN);
    while(1){
        pthread_mutex_lock(&count_lock);
        if (Finished) break;
        size1 = getline(&line1, &Linelen, File1);
        size2 = getline(&line2, &Linelen, File2);
        if (size1 <= 0 && size2 <= 0){
            Finished = true;
            break;
        }
        pthread_mutex_unlock(&count_lock);
        if (size1 != size2){
            if (size1 <= 0){
                printf("> %s", line2);
            }else if (size2 <= 0){
                printf("< %s", line1);
            }else{
                printf("< %s> %s", line1, line2);
            }
        }else{
            if(strcmp(line1, line2)){
                printf("< %s> %s", line1, line2);
            }
        }
    }
    pthread_mutex_unlock(&count_lock);
    free(line1);
    free(line2);
    return NULL;
}
