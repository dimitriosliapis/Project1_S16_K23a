#include "index.h"

#define FOUND 1
#define NOT_FOUND -1
#define H_DEFAULT 64

typedef struct hNode_t{

    uint32_t id;
    int distance;
    uint32_t parent_id;
    struct hNode *next;
}hNode;

typedef struct qNode_t{
    uint32_t id;
    struct q_Node_t *next;
}q_Node;

typedef struct queue_t{
    q_Node *first;
    q_Node *last;
}Queue;

uint32_t q_size;
Queue *queue;
hNode hashTable[H_DEFAULT];

char* append(char *, char*);
char* idToStr(uint32_t);