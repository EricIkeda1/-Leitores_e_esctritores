#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <windows.h> // Inclui a biblioteca windows.h para a função Sleep
#include <time.h> // Inclui a biblioteca time.h para a função time

/* Definição de um produto */
typedef struct {
    int id;
    char cor[20];
    char tamanho[10];
    int estoque;
} Produto;

/* Banco de dados simulado com um array de produtos */
#define NUM_PRODUTOS 5
Produto banco_dados[NUM_PRODUTOS] = {
    {1, "vermelho", "M", 100},
    {2, "azul", "L", 50},
    {3, "verde", "S", 75},
    {4, "preto", "XL", 20},
    {5, "branco", "M", 150}
};

/* Variáveis de controle */
int num_leitores = 0;
sem_t mutex, db;

/* Função para leitores */
void* leitor(void* arg) {
    int id = *((int*)arg);
    int i; /* Declaração de variável movida para o início do bloco*/

    while (1) {
        sem_wait(&mutex);  /* Pede acesso exclusivo para incrementar num_leitores */
        num_leitores++;
        if (num_leitores == 1) { /* Primeiro leitor bloqueia os escritores */
            sem_wait(&db);
        }
        sem_post(&mutex); /* Libera o acesso a num_leitores */

        /* Leitura do banco de dados */
        printf("Leitor %d está lendo o banco de dados...\n", id);
        for (i = 0; i < NUM_PRODUTOS; i++) {
            printf("Produto %d: cor=%s, tamanho=%s, estoque=%d\n",
                   banco_dados[i].id, banco_dados[i].cor, banco_dados[i].tamanho, banco_dados[i].estoque);
        }
        Sleep(1000); /* Simula tempo de leitura */

        sem_wait(&mutex); /* Pede acesso exclusivo para decrementar num_leitores */
        num_leitores--;
        if (num_leitores == 0) { /* Último leitor libera os escritores */
            sem_post(&db);
        }
        sem_post(&mutex); /* Libera o acesso a num_leitores */

        Sleep(rand() % 5000); /* Espera aleatória antes de ler novamente */
    }

    return NULL;
}

/* Função para escritores */
void* escritor(void* arg) {
    int id = *((int*)arg);
    int produto_id; /* Declaração de variável movida para o início do bloco */

    while (1) {
        sem_wait(&db); /* Pede acesso exclusivo ao banco de dados */

        /* Escrita no banco de dados */
        printf("Escritor %d está escrevendo no banco de dados...\n", id);
        produto_id = rand() % NUM_PRODUTOS;
        banco_dados[produto_id].estoque = rand() % 200; /* Atualiza o estoque de um produto aleatório */
        printf("Escritor %d atualizou Produto %d para estoque %d\n",
               id, banco_dados[produto_id].id, banco_dados[produto_id].estoque);
        Sleep(1000); /* Simula tempo de escrita */

        sem_post(&db); /* Libera o acesso ao banco de dados */

        Sleep(rand() % 5000); /* Espera aleatória antes de escrever novamente */
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    /* Inicializa semáforos */
    sem_init(&mutex, 0, 1);
    sem_init(&db, 0, 1);

    /* Cria threads de leitores e escritores */
    pthread_t leitores[3], escritores[2];
    int ids[5] = {1, 2, 3, 4, 5};
    int i; /* Declaração de variável movida para o início do bloco */

    for (i = 0; i < 3; i++) {
        pthread_create(&leitores[i], NULL, leitor, &ids[i]);
    }

    for (i = 0; i < 2; i++) {
        pthread_create(&escritores[i], NULL, escritor, &ids[3 + i]);
    }

    /* Aguarda as threads (nunca ocorre aqui, pois elas estão em loops infinitos) */
    for (i = 0; i < 3; i++) {
        pthread_join(leitores[i], NULL);
    }
    for (i = 0; i < 2; i++) {
        pthread_join(escritores[i], NULL);
    }

    /* Destroi semáforos */
    sem_destroy(&mutex);
    sem_destroy(&db);

    return 0;
}
