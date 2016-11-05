#include "index.h"

#define FOUND 1
#define NOT_FOUND -1
#define H_DEFAULT 64


typedef struct hNode_t {
    uint32_t id;
    uint32_t steps;
    struct hNode *next;
} hNode;

typedef struct qNode_t {
    uint32_t id;
    int steps;
    struct qNode_t *next;
} q_Node;

typedef struct queue_t {
    q_Node *first;
    q_Node *last;
} Queue;

Queue *createQueue();

int isEmpty(Queue *);

int push(Queue *, uint32_t, ind *, int);

uint32_t pop(Queue *, ind *, int *);

int search(Queue *, uint32_t);

uint32_t hash(uint32_t);

int isIn(hNode *, uint32_t);

void addToHash(hNode *, uint32_t, uint32_t);

void resetHash(hNode *);

int bBFS(ind *, ind *, list_node *, list_node *, uint32_t, uint32_t);
