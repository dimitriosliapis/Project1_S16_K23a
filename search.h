#include "index.h"

#define HT_SIZE 131072

#define FOUND 1
#define NOT_FOUND -1

typedef struct htNode_t {
    uint32_t id;
    struct htNode_t *next;
} ht_Node;

typedef struct qNode_t {
    uint32_t id;
    struct qNode_t *next;
} q_Node;

typedef struct queue_t {
    q_Node *first;
    q_Node *last;
} Queue;

ht_Node **createHashtable();

int search(ht_Node **hashTable, uint32_t id);

void insert(ht_Node **hashTable, uint32_t id);

void delete(ht_Node **hashTable);

Queue *createQueue();

int isEmpty(Queue *);

int push(Queue *, uint32_t);

uint32_t pop(Queue *);

void empty(Queue *);

int bBFS(ind *, ind *, list_node *, list_node *, uint32_t, uint32_t);
