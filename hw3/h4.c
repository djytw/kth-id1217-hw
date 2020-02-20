
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#define MAXWORKERS 64
void *Man(void *);
void *Woman(void *);

sem_t e, man, woman;
int nm, nw, dm, dw;
struct timeval start, end;

int main(int argc, char *argv[]) {

  pthread_t wid[MAXWORKERS], mid[MAXWORKERS];

  int num;
  num = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
  if (num > MAXWORKERS) num = MAXWORKERS;
  
  sem_init(&e, 1, 0);
  sem_init(&man, 1, 0);
  sem_init(&woman, 1, 0);

  printf("main started\n");
  long i;
  for (i = 0; i < num; i++){
    pthread_create(&wid[i], NULL, Woman, (void *) i);
    pthread_create(&mid[i], NULL, Man, (void *) i);
  }
  
  gettimeofday( &start, NULL );
  sem_post(&e);
  sleep(1);
}

void *Man(void *arg) {
  long myid = (long) arg;
  while(1){
    sem_wait(&e);
    if (nw > 0 || dw > 0) {
        dm ++; 
        gettimeofday( &end, NULL );
        printf("[%03dms]Man#%ld waits!\n",(int)(1000*((end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec))),myid);
        sem_post(&e); 
        sem_wait(&man);
    }
    nm ++;
    if (dm > 0) {
        dm --; 
        sem_post(&man);
    }else{
        sem_post(&e);
    }

    // take the shower
    gettimeofday( &end, NULL );
    printf("\e[0;33m[%03dms]Man#%ld starts shower!\e[0m\n",(int)(1000*((end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec))),myid);
    int time = (rand()%99+100);
    usleep(time*1000);
    gettimeofday( &end, NULL );
    printf("\e[0;32m[%03dms]Man#%ld finished shower for %dms!\e[0m\n",(int)(1000*((end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec))),myid,time);


    sem_wait(&e);
    nm --;
    if (nm == 0 && dw > 0) {
        dw --; 
        sem_post(&woman);
    }
    else sem_post(&e);
  }
}

void *Woman(void *arg) {
  long myid = (long) arg;
  while(1){
    sem_wait(&e);
    if (nm > 0 || dm > 0) {
        dw ++; 
        gettimeofday( &end, NULL );
        printf("[%03dms]Woman#%ld waits!\n",(int)(1000*((end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec))),myid);
        sem_post(&e); 
        sem_wait(&woman);
    }
    nw ++;
    if (dw > 0) {dw --; sem_post(&woman);}
    else sem_post(&e);

    // take the shower
    gettimeofday( &end, NULL );
    printf("\e[0;33m[%03dms]Woman#%ld starts shower!\e[0m\n",(int)(1000*((end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec))),myid);
    int time = (rand()%99+100);
    usleep(time*1000);
    gettimeofday( &end, NULL );
    printf("\e[0;32m[%03dms]Woman#%ld finished shower for %dms!\e[0m\n",(int)(1000*((end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec))),myid,time);

    sem_wait(&e);
    nw --;
    if (nw == 0 && dm > 0) {dm --; sem_post(&man);}
    else sem_post(&e);
  }
}
