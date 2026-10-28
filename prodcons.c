/* Exercício 3 - Produtor/Consumidor com buffer circular
 * 1 produtor (a cada 1s) e 1 consumidor (a cada 2s), MAXFILA = 8, 64 itens.
 * Compilar: gcc prodcons.c -o prodcons -lpthread
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAXFILA 8
#define TOTAL   64

#ifndef T_PROD
#define T_PROD 1000000   /* 1 s, em microssegundos */
#endif
#ifndef T_CONS
#define T_CONS 2000000   /* 2 s */
#endif

/* Variáveis compartilhadas (globais) */
int fila[MAXFILA];
int inicio = 0;   /* próxima posição a ser lida   */
int fim    = 0;   /* próxima posição a ser escrita */
int qtd    = 0;   /* quantos elementos há na fila  */

pthread_mutex_t mutex       = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  pode_produzir = PTHREAD_COND_INITIALIZER; /* fila não cheia */
pthread_cond_t  pode_consumir = PTHREAD_COND_INITIALIZER; /* fila não vazia */

void *produtor(void *arg) {
    unsigned int seed = (unsigned int) time(NULL);
    (void) arg;

    for (int i = 1; i <= TOTAL; i++) {
        usleep(T_PROD);
        int valor = rand_r(&seed) % 1000;   /* inteiro pseudo-aleatório */

        pthread_mutex_lock(&mutex);
        while (qtd == MAXFILA)              /* fila cheia: espera */
            pthread_cond_wait(&pode_produzir, &mutex);

        fila[fim] = valor;
        fim = (fim + 1) % MAXFILA;
        qtd++;
        printf("[P] item %2d produzido: %3d  (fila: %d/%d)\n",
               i, valor, qtd, MAXFILA);

        pthread_cond_signal(&pode_consumir);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumidor(void *arg) {
    (void) arg;

    for (int i = 1; i <= TOTAL; i++) {
        usleep(T_CONS);

        pthread_mutex_lock(&mutex);
        while (qtd == 0)                    /* fila vazia: espera */
            pthread_cond_wait(&pode_consumir, &mutex);

        int valor = fila[inicio];
        inicio = (inicio + 1) % MAXFILA;
        qtd--;
        printf("    [C] item %2d consumido: %3d  (fila: %d/%d)\n",
               i, valor, qtd, MAXFILA);

        pthread_cond_signal(&pode_produzir);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t tp, tc;

    pthread_create(&tp, NULL, produtor, NULL);
    pthread_create(&tc, NULL, consumidor, NULL);

    pthread_join(tp, NULL);
    pthread_join(tc, NULL);

    printf("Fim: %d itens produzidos e consumidos.\n", TOTAL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&pode_produzir);
    pthread_cond_destroy(&pode_consumir);
    return 0;
}
