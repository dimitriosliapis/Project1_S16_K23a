#ifndef INDEX_H
#define INDEX_H

#include "buffer.h"
#include "hash.h"
#include "thread.h"

#define IND_SIZE 512
#define NUM_GRAIL 5

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

typedef struct static_data_t{
    uint32_t lowlink;
    uint32_t index;
    uint32_t *rank;
    uint32_t *min_rank;
    uint32_t all_children_in_scc[NUM_GRAIL];
    uint32_t children_in_scc;
    list_node *curr_neighbors;
    uint32_t *next_child;
    uint32_t n;
    int onStack;
}static_data;

typedef struct index_t{
    ptrdiff_t first;
    ptrdiff_t last;
    int visited[THREAD_POOL_SIZE + 1];
    ht_Node *neighbors;
    uint32_t num_of_children;
    static_data *s_data;
}ind;

ind *createNodeIndex(uint32_t, int);

int lookup(ind*, uint32_t, uint32_t);

ptrdiff_t insertNode(ind**, uint32_t, list_node**, uint32_t*, uint32_t*, ptrdiff_t*, int);

int reallocNodeIndex(ind**, int, uint32_t*, int);

ptrdiff_t addEdge(ind**, uint32_t, uint32_t, list_node**, uint32_t*, ptrdiff_t*, int, uint32_t);

ptrdiff_t getListHead(ind*, uint32_t);

int destroyNodeIndex(ind*, uint32_t index_size, int s);

#endif
