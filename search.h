#include "index.h"

#define QUEUE_SIZE 65536

typedef struct queue_t {
    int size;
    int * ids;
    int first;
    int last;
    int count;
} Queue;

Queue *createQueue();

int isEmpty(Queue *);

int enq(Queue *, int);

int deq(Queue *);

void restartQueue(Queue *);

void empty(Queue *);

int bBFS(ind *, ind *, list_node *, list_node *, int, int, Queue *, Queue *, int, int, int);


