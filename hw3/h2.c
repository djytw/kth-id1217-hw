
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAXWORKERS 64
void *Bear(void *);
void *Honeybees(void *);

sem_t busy, full;
int W = 5;
int pot;

int main(int argc, char *argv[]) {

  pthread_t pid, bid[MAXWORKERS];

  int numBees;
  numBees = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
  if (numBees > MAXWORKERS) numBees = MAXWORKERS;
  
  W = (argc > 2)? atoi(argv[2]) : W;
  pot = 0;
  sem_init(&busy, 1, 0);
  sem_init(&full, 1, 0);

  printf("main started\n");
  pthread_create(&pid, NULL, Bear, NULL);
  long i;
  for (i = 0; i < numBees; i++)
  pthread_create(&bid[i], NULL, Honeybees, (void *) i);

  sem_post(&busy);
  sleep(1);
}

void *Honeybees(void *arg) {
  long myid = (long) arg;
  while(1){
    sem_wait(&busy);
    if (pot < W) {
      pot++; //fill the pot by 1
      printf("#%ld ", myid);
      sem_post(&busy);
      usleep(100000); //time to find honey?
    } else {
      sem_post(&full); //awake the bear
    }
  }
}

void *Bear(void *arg) {
  while(1) {
    sem_wait(&full);
    pot = 0; //eats all the honey
    printf("\nBear!\n");
    sem_post(&busy); //make the pot available again
  }
}
