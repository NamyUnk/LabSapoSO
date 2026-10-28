/* Exercício 4 - Produtor/Consumidor com VÁRIOS produtores e consumidores
 * NPROD produtores e NCONS consumidores dividem os 64 itens.
 * Compilar: gcc prodcons_multi.c -o prodcons_multi -lpthread
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAXFILA 8
#define TOTAL   64
#define NPROD   2
#define NCONS   2

#ifndef T_PROD
#define T_PROD 1000000
#endif
#ifndef T_CONS
#define T_CONS 2000000
#endif

int fila[MAXFILA];
int inicio = 0, fim = 0, qtd = 0;

pthread_mutex_t mutex         = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  pode_produzir = PTHREAD_COND_INITIALIZER;
pthread_cond_t  pode_consumir = PTHREAD_COND_INITIALIZER;

void *produtor(void *arg) {
    long id = (long) arg;
    unsigned int seed = (unsigned int) time(NULL) + (unsigned int) id * 7919;
    int minha_cota = TOTAL / NPROD;   /* cada produtor faz 64/NPROD itens */

    for (int i = 1; i <= minha_cota; i++) {
        usleep(T_PROD);
        int valor = rand_r(&seed) % 1000;

        pthread_mutex_lock(&mutex);
        /* WHILE é obrigatório: outro produtor pode ter enchido a fila
         * entre o signal e o momento em que esta thread reacorda. */
        while (qtd == MAXFILA)
            pthread_cond_wait(&pode_produzir, &mutex);

        fila[fim] = valor;
        fim = (fim + 1) % MAXFILA;
        qtd++;
        printf("[P%ld] produziu %3d  (fila: %d/%d)\n", id, valor, qtd, MAXFILA);

        pthread_cond_signal(&pode_consumir);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumidor(void *arg) {
    long id = (long) arg;
    int minha_cota = TOTAL / NCONS;   /* cada consumidor tira 64/NCONS itens */

    for (int i = 1; i <= minha_cota; i++) {
        usleep(T_CONS);

        pthread_mutex_lock(&mutex);
        /* WHILE: outro consumidor pode ter pego o item antes desta thread. */
        while (qtd == 0)
            pthread_cond_wait(&pode_consumir, &mutex);

        int valor = fila[inicio];
        inicio = (inicio + 1) % MAXFILA;
        qtd--;
        printf("        [C%ld] consumiu %3d  (fila: %d/%d)\n",
               id, valor, qtd, MAXFILA);

        pthread_cond_signal(&pode_produzir);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t prod[NPROD], cons[NCONS];

    for (long i = 0; i < NPROD; i++) pthread_create(&prod[i], NULL, produtor, (void *) i);
    for (long i = 0; i < NCONS; i++) pthread_create(&cons[i], NULL, consumidor, (void *) i);

    for (int i = 0; i < NPROD; i++) pthread_join(prod[i], NULL);
    for (int i = 0; i < NCONS; i++) pthread_join(cons[i], NULL);

    printf("Fim: %d itens produzidos e consumidos.\n", TOTAL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&pode_produzir);
    pthread_cond_destroy(&pode_consumir);
    return 0;
}
