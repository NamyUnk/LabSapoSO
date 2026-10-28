/* Exercício 2 - Corrida de Sapos COM mutex
 * Compilar: gcc sapos_mutex.c -o sapos_mutex -lpthread
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 5
#define PULO_MAXIMO 100
#define DESCANSO_MAXIMO 1
#define DISTANCIA_PARA_CORRER 100

static int classificacao = 1;
static int cont = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *Correr(void *sapo) {
    int pulos = 0;
    int distanciaJaCorrida = 0;

    while (distanciaJaCorrida <= DISTANCIA_PARA_CORRER) {
        int pulo = rand() % PULO_MAXIMO;
        distanciaJaCorrida += pulo;
        pulos++;
        printf("Sapo %ld pulou\n", (long) sapo);
        int descanso = rand() % DESCANSO_MAXIMO;
        sleep(descanso);
    }

    /* Região crítica: só um sapo por vez pode "cruzar a linha de chegada".
     * Ler a posição, imprimir e incrementar viram uma operação atômica. */
    pthread_mutex_lock(&lock);
    printf("Sapo %ld chegou na posicao %d com %d pulos\n",
           (long) sapo, classificacao, pulos);
    cont++;
    classificacao++;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    long t;

    classificacao = 1;
    printf("Corrida iniciada ...\n");

    for (t = 0; t < NUM_THREADS; t++)
        pthread_create(&threads[t], NULL, Correr, (void *) t);
    for (t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    printf("\nAcabou!! (%d sapos chegaram)\n", cont);
    pthread_mutex_destroy(&lock);
    pthread_exit(NULL);
}
