#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


typedef struct{
    int n; 
    int count; 
    pthread_mutex_t mimutex;
    pthread_cond_t condicion;
}Monitor;


Monitor barrera;
Monitor ciclo;
int value = 10;

void init_monitor(Monitor* m, int num){
    m->n = num; 
    m->count = 0;
}

void wait_monitor(Monitor* m){
    pthread_mutex_lock(&m->mimutex);
    m->count++;
    if(m->count == m->n){
        m->count=0;
        pthread_cond_broadcast(&m->condicion); //notifica a todas las hebras que esperan la condicion
    }
    else{
        pthread_cond_wait(&m->condicion, &m->mimutex); //asigna a la hebra que espera hasta que se cumpla la condicion 

    }
    pthread_mutex_unlock(&m->mimutex);
}


struct arg_struct{
    int thread;
    int m_value;
};



void* fun(void* arguments){
    struct arg_struct *args = (struct arg_struct*) arguments;
    for(int i = 0; i < args->m_value; i++){
        int a = value;
        value++;
        sleep(rand()%5+2);
        wait_monitor(&barrera);
        wait_monitor(&ciclo);
        printf("thread:  %d, value = %d\n", args->thread, a); 
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    srand(time(NULL));
	if(argc != 3) return 0; 
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    init_monitor(&barrera, N+1);
    init_monitor(&ciclo, N+1);
 
    struct arg_struct args[N];
    pthread_t h[N];

    for(int i = 0; i < N; i++){
        args[i].thread = i; 
        args[i].m_value = M;
        if(pthread_create(&h[i], NULL,  &fun, (void*)&args[i]) != 0) perror("Error 1");
    }

    for(int i = 0; i < M; i++){
        wait_monitor(&barrera); //espera a que se realicen todas las hebras
        //sleep(1);
        printf("\nEtapa %d Completa \n", i+1);
        wait_monitor(&ciclo);  //imprime todo junto 
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(h[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

	return 0;
}