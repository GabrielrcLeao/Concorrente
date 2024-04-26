# Concorrente

Threads Envolvidas:
- Compradores: Representados pela função comprar_ingressos. Cada comprador é uma thread separada que tenta comprar ingressos para um evento. O número de compradores é definido por NUM_COMPRADORES.
- Geradores: Representados pela função gerar_ingressos. São responsáveis por adicionar ingressos ao estoque disponível. O número de geradores é definido por NUM_GERADORES.

Sincronização entre as Threads:
- Mutex : Utilizado para garantir a exclusão mútua ao acessar recursos compartilhados, como o número de ingressos disponíveis e a fila de compradores.
- Variáveis de Condição (cond_not_full e cond_not_empty): Usadas para sincronizar compradores e geradores:
- cond_not_full: Espera um sinal para quando houver espaço disponível na fila de compradores.
- cond_not_empty: Espera um sinal para quando houver ingressos disponíveis para compra.

Mecanismo de Feedback de Reserva:
- Quando um comprador tenta comprar ingressos e não há ingressos suficientes disponíveis, ele aguarda na fila até que mais ingressos estejam disponíveis (pthread_cond_wait(&cond_not_empty, &mutex)).
- Quando um comprador conclui com sucesso uma compra, ele adiciona sua compra à fila de compradores e sinaliza aos geradores que há espaço disponível na fila (pthread_cond_signal(&cond_not_full)).
- Quando um gerador adiciona ingressos ao estoque, ele sinaliza aos compradores que há ingressos disponíveis para compra (pthread_cond_signal(&cond_not_empty)).
