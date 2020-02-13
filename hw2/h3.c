
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 25143
#define MAXWORKERS 8

int numWorkers;
int size; 
void *Worker(void *);
char dict[25143][24];

int cmpfunc(const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}
int main(int argc, char *argv[]) {    
    FILE* fin = fopen("words","r");
    int i;
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

    for (i = 0; i < MAXSIZE; i++){
        fgets(dict[i], 24, fin);
        char *pos;
        if ((pos=strchr(dict[i], '\n')) != NULL)
            *pos = '\0';
    }
    
    omp_set_num_threads(numWorkers);
    int palin[1000];
    int palintop = 0;

    start_time = omp_get_wtime();
    #pragma omp parallel for
    for (i = 0; i < size; i++){
        int l = strlen(dict[i]);
        int j;
        char str[24];
        for (j = 0; j < l; j++){
            str[j] = dict[i][l - 1 - j];
        }
        str[j] = 0;
        for (j = 0; j < MAXSIZE; j++){
            if (!strcasecmp(str, dict[j])){
                int top;
                #pragma omp critical 
                top = palintop++;
                //top = __sync_fetch_and_add(&palintop, 1);
                palin[top] = j;
                break;
            }
        }
    }
    end_time = omp_get_wtime();

    qsort(palin, palintop, sizeof(int), cmpfunc);
    FILE* fo = fopen("h3.out", "w+");
    fprintf(fo, "Total %d palindro words\n", palintop);
    printf("Total %d palindro words\n", palintop);
    printf("The execution time is %g sec\n", end_time - start_time);
    for (i = 0; i < palintop; i++){
        fprintf(fo,"%s\n",dict[palin[i]]);
    }
    fclose(fo);
    fclose(fin);
    return 0;
}