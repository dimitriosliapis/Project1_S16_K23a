#include "buffer.h"
#include "hash.h"

#define IND_SIZE 262144

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

typedef struct index_t{
    ptrdiff_t first;
    ptrdiff_t last;
    uint32_t max;
    uint32_t lowlink;
    uint32_t index;
    uint32_t rank;
    uint32_t min_rank;
    ht_Node *neighbors;
    uint32_t all_children_in_scc;
    uint32_t num_of_children;
}ind;

ind *createNodeIndex(uint32_t);

int lookup(ind*, uint32_t, uint32_t);

ptrdiff_t insertNode(ind**, uint32_t, list_node**, uint32_t*, uint32_t*, ptrdiff_t*);

int reallocNodeIndex(ind**, int, uint32_t*);

ptrdiff_t addEdge(ind**, uint32_t, uint32_t, list_node**, uint32_t*, ptrdiff_t*);

ptrdiff_t getListHead(ind*, uint32_t);

int destroyNodeIndex(ind*, uint32_t index_size);
