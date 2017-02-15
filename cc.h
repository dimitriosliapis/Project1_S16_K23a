#ifndef CC_H
#define CC_H

#include "search.h"

#define INIT_UNODE_SIZE 64
#define INIT_NEWNODE_SIZE 32

#define METRIC 100
#define STACK_ARRAY_SIZE 2048

typedef struct stack {
    uint32_t *stack_array;
    int size;
    int last;
    int count;
} Stack_t;

Stack_t *createStack();

int stackisempty(Stack_t *stack);

void pushinstack(Stack_t *stack, uint32_t id);

uint32_t popfromstack(Stack_t *stack);

uint32_t peekfromstack(Stack_t *stack);

void deletestack(Stack_t *stack);


typedef struct CC_ind_t {
    uint32_t id;
    int visited[THREAD_POOL_SIZE + 1];
} CC_ind;

typedef struct u_unode_t {
    uint32_t *cc_array;     // array with the cc
    uint32_t size;          // cc_array size
    uint32_t *new_nodes;    // array with new nodes to each cc and they weren't in the graph before
    uint32_t n_size;        // new_nodes size
    char state;             // o -> old, e -> empty, n -> new
    int visited[THREAD_POOL_SIZE + 1];
} u_node;

typedef struct CC_t {
    CC_ind *cc_index;
    uint32_t cc_size;       // megethos cc_index
    uint32_t cc_max;
    u_node *updateIndex;
    uint32_t u_size;        // megethos updateIndex
    uint32_t metricVal;
} CC;

CC *createCCIndex(uint32_t, ind *, ind *, list_node *, list_node *, uint32_t, uint32_t, uint32_t);

void initUpdateIndex(CC *);

void refreshUpdateIndex(CC *, uint32_t, uint32_t);

int searchUpdateIndex(CC, uint32_t, uint32_t, uint32_t, int);

uint32_t updateCCIndex(CC *, uint32_t, uint32_t, int);

void destroyCCIndex(CC *);

#endif
