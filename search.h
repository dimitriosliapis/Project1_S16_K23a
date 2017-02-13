#ifndef SEARCH_H
#define SEARCH_H

#include "index.h"

#define QUEUE_SIZE 512

typedef struct queue_t {
    int size;
    uint32_t * ids;
    int first;
    int last;
    int count;
} Queue;

Queue *createQueue();

int isEmpty(Queue *);

int enq(Queue *, uint32_t);

uint32_t deq(Queue *);

void restartQueue(Queue *);

void empty(Queue *);

int bBFS(ind *, ind *, list_node *, list_node *, uint32_t, uint32_t, Queue *, Queue *, uint32_t, int, int);

#endif
