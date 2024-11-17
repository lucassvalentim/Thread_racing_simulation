#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define N 10

/*
    global definitions for controlling critical regions and launching threads
    at the same time and stop execution
*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 1;
bool stop_flag = false;

void * print_thread_id(void *idArg){
    int id = *((int*)(idArg));

    
    //critical thread launch region.
    pthread_mutex_lock(&mutex);

    while(!ready){
        pthread_cond_wait(&cond, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    
    // thread race
    for(int i = 1; i <= 100; i++){
        pthread_mutex_lock(&mutex);
        if(stop_flag){
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        if(i == 100){
            stop_flag = true;
            printf("Thread %d is calculating %d is WINNER!\n", id, i);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        printf("Thread %d is calculating %d\n", id, i);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(){

    pthread_t threads[N];
    int idThread[N];
    for(int i = 0; i < N; i++){
        idThread[i] = i;
        pthread_create(&threads[i], NULL, print_thread_id, (void*) &idThread[i]);
    }

    // condition that releases the threads
    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    printf("All thread completed\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;   
}