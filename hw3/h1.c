
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAXWORKERS 64
void *parentBird(void *);
void *babyBird(void *);

sem_t busy, empty;
int W = 5;
int dish;

int main(int argc, char *argv[]) {

  pthread_t pid, bid[MAXWORKERS];

  int numBirds;
  numBirds = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
  if (numBirds > MAXWORKERS) numBirds = MAXWORKERS;
  
  W = (argc > 2)? atoi(argv[2]) : W;
  dish = W;
  sem_init(&busy, 1, 0);
  sem_init(&empty, 1, 0);

  printf("main started\n");
  pthread_create(&pid, NULL, parentBird, NULL);
  long i;
  for (i = 0; i < numBirds; i++)
  pthread_create(&bid[i], NULL, babyBird, (void *) i);

  sem_post(&busy);
  sleep(1);
}

void *babyBird(void *arg) {
  long myid = (long) arg;
  while(1){
    sem_wait(&busy);
    if (dish > 0) {
      dish--; //eat a worm
      printf("#%ld ", myid);
      sem_post(&busy);
      usleep(100000); //sleep a bit
    } else {
      sem_post(&empty); //chirps the parent bird
    }
  }
}

void *parentBird(void *arg) {
  while(1) {
    sem_wait(&empty);
    dish = W; //refill the dish
    printf("\nparentBird!\n");
    sem_post(&busy); //make the dish available again
  }
}
