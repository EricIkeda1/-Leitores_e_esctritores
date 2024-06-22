#include <stdio.h> /* Inclui a biblioteca padrao de entrada e saída (para funcoes como printf) */
#include <stdlib.h> /* Inclui a biblioteca padrao para funções utilitárias (como rand, srand) */
#include <pthread.h> /* Inclui a biblioteca para manipulacao de threads */
#include <semaphore.h> /* Inclui a biblioteca para manipulação de semáforos */
#include <windows.h> /* Inclui a biblioteca para funções do Windows (como Sleep) */
#include <time.h> /* Inclui a biblioteca para manipulação de tempo (como time e srand) */

/* Definição da estrutura do Produto */
typedef struct {
    int id; /* ID do produto (identificação única) */
    char cor[20]; /* Cor do produto */
    char tamanho[10]; /* Tamanho do produto (ex: S, M, L, XL) */
    int estoque; /* Quantidade em estoque do produto */
} Produto;

/* Banco de dados simulado com produtos */
#define NUM_PRODUTOS 5 /* Define o numero de produtos no banco de dados */
Produto banco_dados[NUM_PRODUTOS] = {
    {1, "vermelho", "M", 100}, /* Produto 1 */
    {2, "azul", "L", 50}, /* Produto 2 */
    {3, "verde", "S", 75}, /* Produto 3 */
    {4, "preto", "XL", 20}, /* Produto 4 */
    {5, "branco", "M", 150} /* Produto 5 */
};

/* Variáveis de controle para os semáforos */
int num_leitores = 0; /* Contador de leitores (número de threads leitoras ativas) */
sem_t mutex, db; /* Semáforos para controle de acesso */

/* Função que define o comportamento de um leitor */
void *leitor(void *arg) {
    int id = *((int *)arg); /* ID do leitor, passado como argumento */
    int i; /* Variável de controle para loop */

    while (1) { /* Loop infinito para simular leituras contínuas */
        sem_wait(&mutex); /* Entra na secao critica para incrementar o contador de leitores */
        num_leitores++; /* Incrementa o contador de leitores */
        if (num_leitores == 1) { /* Se for o primeiro leitor */
            sem_wait(&db); /* Bloqueia o acesso para escritores */
        }
        sem_post(&mutex); /* Sai da seção critica */

        /* Leitura do banco de dados */
        printf("Leitor %d est� lendo o banco de dados...\n", id); /* Indica que o leitor está lendo */
        for (i = 0; i < NUM_PRODUTOS; i++) { /* Itera sobre os produtos no banco de dados */
            /* Imprime os detalhes de cada produto */
            printf("Produto %d: cor=%s, tamanho=%s, estoque=%d\n",
                   banco_dados[i].id, banco_dados[i].cor, banco_dados[i].tamanho, banco_dados[i].estoque);
        }
        Sleep(1000); /* Simula o tempo de leitura com uma espera de 1 segundo */

        sem_wait(&mutex); /* Entra na seção crítica para decrementar o contador de leitores */
        num_leitores--; /* Decrementa o contador de leitores */
        if (num_leitores == 0) { /* Se for o último leitor */
            sem_post(&db); /* Libera o acesso para escritores */
        }
        sem_post(&mutex); /* Sai da seção crítica */

        Sleep(rand() % 5000); /* Espera aleatária antes de ler novamente (0 a 5 segundos) */
    }

    return NULL; /* Retorno nulo, não utilizado */
}

/* Função que define o comportamento de um escritor */
void *escritor(void *arg) {
    int id = *((int *)arg); /* ID do escritor, passado como argumento */
    int produto_id; /* ID do produto a ser atualizado */

    while (1) { /* Loop infinito para simular escritas contínuas */
        sem_wait(&db); /* Bloqueia o acesso exclusivo ao banco de dados */

        /* Escrita no banco de dados */
        printf("Escritor %d est� escrevendo no banco de dados...\n", id); /* Indica que o escritor está escrevendo */
        produto_id = rand() % NUM_PRODUTOS; /* Escolhe um produto aleatório para atualizar */
        banco_dados[produto_id].estoque = rand() % 200; /* Atualiza o estoque do produto com um valor aleatório */
        printf("Escritor %d atualizou Produto %d para estoque %d\n",
               id, banco_dados[produto_id].id, banco_dados[produto_id].estoque);
        Sleep(1000); /* Simula o tempo de escrita com uma espera de 1 segundo */

        sem_post(&db); /* Libera o acesso ao banco de dados */

        Sleep(rand() % 5000); /* Espera aleatória antes de escrever novamente (0 a 5 segundos) */
    }

    return NULL; /* Retorno nulo, não utilizado */
}

int main() {
    srand(time(NULL)); /* Inicializa a semente do gerador de números aleatórios */

    /* Inicialização dos semáforos */
    sem_init(&mutex, 0, 1); /* Inicializa o semáforo mutex com valor 1 */
    sem_init(&db, 0, 1); /* Inicializa o semáforo db com valor 1 */

    /* Criação de threads de leitores e escritores */
    pthread_t leitores[3], escritores[2]; /* Arrays para armazenar os identificadores das threads de leitores e escritores */
    int ids[5] = {1, 2, 3, 4, 5}; /* IDs para leitores e escritores */
    int i; /* Variável de controle para loop */

    /* Criação das threads de leitores */
    for (i = 0; i < 3; i++) {
        pthread_create(&leitores[i], NULL, leitor, &ids[i]); /* Cria uma thread de leitor para cada ID */
    }

    /* Criação das threads de escritores */
    for (i = 0; i < 2; i++) {
        pthread_create(&escritores[i], NULL, escritor, &ids[3 + i]); /* Cria uma thread de escritor para cada ID */
    }

    /* Aguarda a conclusão das threads (nunca ocorre aqui, pois elas estão em loops infinitos) */
    for (i = 0; i < 3; i++) {
        pthread_join(leitores[i], NULL); /* Aguarda a conclusão das threads de leitores */
    }
    for (i = 0; i < 2; i++) {
        pthread_join(escritores[i], NULL); /* Aguarda a conclusão das threads de escritores */
    }

    /* Destruição dos semáforos */
    sem_destroy(&mutex); /* Destroi o semáforo mutex */
    sem_destroy(&db); /* Destroi o semáforo db */

    return 0; /* Retorno do programa principal */
}
