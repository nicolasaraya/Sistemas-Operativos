//g++ -o alarm_clock alarm_clock.cpp -pthread -std=c++14
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

pthread_barrier_t ciclo;
pthread_barrier_t barrera;


struct arg_struct {
    int thread;
    int m_value;
};

void* fun(void* arguments){
    struct arg_struct *args = (struct arg_struct*) arguments;
    for(int i = 0; i < args->m_value; i++){
        int a = rand()%100 + 10;
        sleep(rand()%5+2);
        pthread_barrier_wait(&barrera);
        pthread_barrier_wait(&ciclo);
        printf("thread:  %d, value = %d\n", args->thread, a); 
    }
}

int main(int argc, char const *argv[]){
    srand(time(NULL));
	if(argc != 3) return 0; 
    
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    //int N = 10;
    //int M = 2;
    struct arg_struct args[N];
    pthread_t h[N];

    pthread_barrier_init(&barrera, NULL, N+1); //para las N hebras y la hebra main
    pthread_barrier_init(&ciclo, NULL, N+1); 

    for(int i = 0; i < N; i++){
        args[i].thread = i; 
        args[i].m_value = M;
        if(pthread_create(&h[i], NULL,  &fun, (void*)&args[i]) != 0) perror("Error 1");
    }

    for(int i = 0; i < M; i++){
        pthread_barrier_wait(&barrera); //espera a que se realicen todas las hebras
        printf("\nEtapa %d Completa \n", i+1);
        pthread_barrier_wait(&ciclo);  //imprime todo junto 
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(h[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

	return 0;
}