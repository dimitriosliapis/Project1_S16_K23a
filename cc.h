#include "search.h"

#define INIT_UNODE_SIZE 32
#define INIT_NEWNODE_SIZE 64

typedef struct sNode_t{
    uint32_t id;
    struct sNode_t *next;
}sNode;

typedef struct stack_t{
    sNode *last;
}Stack;

typedef struct u_unode_t{
    uint32_t *cc_array;
    uint32_t size;
    uint32_t *new_nodes;
    uint32_t n_size;
    char state;
}u_node;


void push(Stack*, uint32_t);
uint32_t pop(Stack*);
int stackIsEmpty(Stack*);
void deleteStack(Stack*, sNode*);

typedef struct CC_t{
    uint32_t *cc_index; //CCInde
    uint32_t cc_size;
    uint32_t cc_max;
    u_node * updateIndex;
    uint32_t u_size;
    uint32_t metricVal;
}CC;

CC* createCCIndex(uint32_t, ind*, ind*, list_node*, list_node*, uint32_t, uint32_t, ht_Node*, uint32_t);
void initUpdateIndex(CC *);
void refreshUpdateIndex(CC *, uint32_t, uint32_t);
int searchUpdateIndex(CC, uint32_t, uint32_t, ht_Node*, uint32_t);
void updateCCIndex(CC *, ht_Node*, ht_Node*, uint32_t);