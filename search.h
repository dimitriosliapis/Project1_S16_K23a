#include "index.h"
#include "buffer.h"
#include "hash.h"

typedef struct qNode_t {
    uint32_t id;
    struct qNode_t *next;
} q_Node;

typedef struct queue_t {
    q_Node *first;
    q_Node *last;
} Queue;

Queue *createQueue();

int isEmpty(Queue *);

int push(Queue *, uint32_t);

uint32_t pop(Queue *);

void empty(Queue *);

int bBFS(ind *, ind *, list_node *, list_node *, uint32_t, uint32_t);
