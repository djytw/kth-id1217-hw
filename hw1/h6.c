#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdlib.h>
#define MAXWORKERS 24
#define toLowerCase(a) (((a)>='A'&&(a)<='Z')?((a)+'a'-'A'):(a))
#define next(a) (((*(a+1))=='.'||(*(a+1))=='&'||(*(a+1))=='\'')?(a+=2):(a++))


unsigned short palindro[MAXWORKERS][1000], palintop[MAXWORKERS] = {0};
char dict[25143][24];
int Current = 0;
int NumWorkers;
double start_time, end_time;
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
bool findword(char*);
void *Worker(void *);
double read_timer();

int cmpfunc(const void * a, const void * b){
   return ( *(short*)a - *(short*)b );
}


int wordcmp(const char *s1,const char *s2){
    for(;toLowerCase(*s1)==toLowerCase(*s2);next(s1),next(s2))
        if(*s1=='\0')
            return(0);
    return toLowerCase(*s1)-toLowerCase(*s2);
}
int main(int argc, char **argv){
    FILE* fin = fopen("tests/h6","r");
    int i;
    for (i = 0; i < 25143; i++){
        fgets(dict[i], 24, fin);
        char *pos;
        if ((pos=strchr(dict[i], '\n')) != NULL)
            *pos = '\0';
    }
    
    long l;
    pthread_t workerid[MAXWORKERS];
    NumWorkers = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
    if (NumWorkers > MAXWORKERS) NumWorkers = MAXWORKERS;

    
    start_time = read_timer();
    for (l = 0; l < NumWorkers; l++)
        pthread_create(&workerid[l], NULL, Worker, (void*)l);

    unsigned short palin[2000], top = 0;
    for (l = 0; l < NumWorkers; l++){
        pthread_join(workerid[l], NULL);
        memcpy(&palin[top], palindro[l], palintop[l]*sizeof(short));
        top += palintop[l];
    }
    qsort(palin, top, sizeof(short), cmpfunc);
    FILE* fo = fopen("tests/h6.out", "w+");
    fprintf(fo, "Total %d palindro words\n", top);
    printf("Total %d palindro words\n", top);
    for (i = 0; i < top; i++){
        fprintf(fo,"%s\n",dict[palin[i]]);
    }
    fclose(fo);
    fclose(fin);
    return 0;
}
bool findword(char* str){
    int start = 0, end = 25142;
    int t;
    while (end > start){
        t = wordcmp( dict[ (end+start)/2 ], str);
        if (t < 0){
            start = (end+start)/2 + 1;
        }else if (t > 0){
            end = (end+start)/2 - 1;
        }else{
            //printf("Found: %s==%s\n",str, dict[ (end+start)/2 ]);
            return true;
        }
    }
    t = wordcmp( dict[ (end+start)/2 ], str);
    if (t == 0){
        //printf("Found: %s==%s\n",str, dict[ (end+start)/2 ]);
        return true;
    }
    return false;
}
void *Worker(void *arg) {
    long myid = (long) arg;
    int current;
    char str[24];
    palintop[myid] = 0;
    while(1){
        pthread_mutex_lock(&count_lock);
        if (Current > 25142){
            pthread_mutex_unlock(&count_lock);
            break;
        }
        current = Current ++;
        pthread_mutex_unlock(&count_lock);
        
        int i, l = strlen(dict[current]);
        for (i = 0; i < l; i++){
            str[i] = dict[current][l - 1 - i];
        }
        str[i] = 0;
        if (findword(str)){
            palindro[myid][palintop[myid]++] = current;
        }
    }
    printf("Worker #%ld exit, found %d palindro\n", myid, palintop[myid]);
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