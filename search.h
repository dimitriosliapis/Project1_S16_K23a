#include "index.h"

#define QUEUE_SIZE 4096

typedef struct qEntry_t {
    uint32_t id;
    int steps;
} qEntry;

typedef struct queue_t {
    int size;
    qEntry * entries;
    int first;
    int last;
    int count;
} Queue;

Queue *createQueue();

int isEmpty(Queue *);

int enq(Queue *, uint32_t, int);

uint32_t deq(Queue *, int *);

uint32_t qpeek(Queue *);

int nsteps(Queue *queue, uint32_t id);

void restartQueue(Queue *);

void empty(Queue *);

int bBFS(ind *, ind *, list_node *, list_node *, uint32_t, uint32_t, Queue *, Queue *, ht_Node *, ht_Node *, uint32_t);