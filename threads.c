#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_INGRESSOS 15
#define NUM_COMPRADORES 15
#define MAX_INGRESSOS_POR_COMPRA 3
#define NUM_GERADORES 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

int ingressos_disponiveis = NUM_INGRESSOS;
int queue[NUM_COMPRADORES];
int queue_count = 0;

void *comprar_ingressos(void *thread_id) {
  int tid = *((int *)thread_id);
  int ingressos_a_comprar = rand() % MAX_INGRESSOS_POR_COMPRA + 1;

  pthread_mutex_lock(&mutex);

  while (ingressos_a_comprar > ingressos_disponiveis) {
    printf("Comprador %d: Não há ingressos suficientes disponíveis.\n", tid);
    pthread_cond_wait(&cond_not_empty, &mutex);
  }

  if (ingressos_disponiveis > 0) {
    printf("Comprador %d comprou %d ingresso(s).\n", tid, ingressos_a_comprar);
    queue[queue_count++] = ingressos_a_comprar;
    ingressos_disponiveis -= ingressos_a_comprar;
    printf("%d ingressos disponíveis.\n", ingressos_disponiveis);
  } else {
    printf("Comprador %d: Desculpe, não há mais ingressos disponíveis. "
           "Aguardando recarga...\n",
           tid);
  }
  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&cond_not_full);
  pthread_exit(NULL);
}

void *gerar_ingressos(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    while (queue_count == 0) {
      pthread_cond_wait(&cond_not_full, &mutex);
    }
    int ingressos_adicionados = +rand() % 4;
    ingressos_disponiveis += ingressos_adicionados;
    printf("%d ingressos foram adicionados. %d ingressos disponíveis.\n",
           ingressos_adicionados, ingressos_disponiveis);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_not_empty);
    sleep(rand() % 5);
  }
}

int main() {
  pthread_t compradores[NUM_COMPRADORES];
  pthread_t geradores[NUM_GERADORES];

  srand(time(NULL));

  for (int i = 0; i < NUM_COMPRADORES; i++) {
    int *comprador_id = malloc(sizeof(int));
    *comprador_id = i + 1;
    pthread_create(&compradores[i], NULL, comprar_ingressos, comprador_id);
  }

  for (int i = 0; i < NUM_GERADORES; i++) {
    pthread_create(&geradores[i], NULL, gerar_ingressos, NULL);
  }

  for (int i = 0; i < NUM_COMPRADORES; i++) {
    pthread_join(compradores[i], NULL);
  }

  for (int i = 0; i < NUM_GERADORES; i++) {
    pthread_cancel(geradores[i]);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_not_full);
  pthread_cond_destroy(&cond_not_empty);

  return 0;
}
