#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define NTHREAD 5

/*
    global definitions for controlling critical regions and launching threads
    at the same time and stop execution
*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// flag to release all threads at the same time
bool ready = false;

// global definitions for pilot positions
int runners[NTHREAD];
int pos_runners = 0;

void * car_race(void *idArg){
    int id = *((int*)(idArg));

    // critical thread launch region.
    pthread_mutex_lock(&mutex);
    while(!ready) 
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    // critical region to simulate the race
    pthread_mutex_lock(&mutex);
    runners[pos_runners++] = id;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(){
    pthread_t threads[NTHREAD];
    int id_thread[NTHREAD];
    for(int i = 1; i <= NTHREAD; i++){
        id_thread[i - 1] = i;
        pthread_create(&threads[i-1], NULL, car_race, &id_thread[i-1]);
    }

    // condition that releases the threads
    pthread_mutex_lock(&mutex);
    ready = true;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < NTHREAD; i++){
        pthread_join(threads[i], NULL);
    }

    for(int i = 1; i <= NTHREAD; i++){
        printf("%d: piloto %d\n", i, runners[i-1]);
    }

    printf("Race finished\n");
    return 0;
}