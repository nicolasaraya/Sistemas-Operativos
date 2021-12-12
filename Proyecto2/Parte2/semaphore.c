#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


struct arg_struct {
    int thread;
    int m_value;
};

typedef struct {
    int n; 
    int count;
    sem_t mutex;
    sem_t s1;
    sem_t s2;
}barrier;

void barr_init(barrier * b, int n){
    b->n = n; 
    b->count = 0; 
    sem_init(&b->mutex, 0, 1);
    sem_init(&b->s1, 0, 0);
    sem_init(&b->s2, 0, 1);
}

void wait_barr(barrier *b){
    sem_wait(&b->mutex);
    b->count++;
 
    if (b->count == b->n) {
        sem_wait(&b->s2);
        sem_post(&b->s1);    
    }
    sem_post(&b->mutex);

    sem_wait(&b->s1);
    sem_post(&b->s1);
    
    sem_wait(&b->mutex);
    b->count = b->count -1;

    if(b->count==0){
        sem_wait(&b->s1);
        sem_post(&b->s2);            
    }
    sem_post(&b->mutex);

    sem_wait(&b->s2);
    sem_post(&b->s2);
}


barrier b1, b2; 


void* fun(void* arguments){
    printf("Thread creado\n");
    struct arg_struct *args = (struct arg_struct*) arguments;
    for(int i = 0; i < args->m_value; i++){
        int a = rand()%100 + 10;
        wait_barr(&b1);
        wait_barr(&b2);
        printf("thread:  %d, value = %d\n", args->thread, a); 
    }
}

int main(int argc, char const *argv[]){
    srand(time(NULL));
    if(argc != 3) return 0; 
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    struct arg_struct args[N];
    pthread_t h[N];
    barr_init(&b1, N+1);
    barr_init(&b2, N+1);

    for(int i = 0; i < N; i++){
        args[i].thread = i; 
        args[i].m_value = M;
        if(pthread_create(&h[i], NULL,  &fun, (void*)&args[i]) != 0) perror("Error 1");
    }
    for(int i = 0; i < M; i++){
        wait_barr(&b1);
        sleep(1);
        printf("\nEtapa %d Completa \n", i+1);
        wait_barr(&b2);
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(h[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    return 0;
}