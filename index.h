#include "buffer.h"

#define IND_SIZE 2048

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

typedef struct index_t{
    ptrdiff_t first;
    ptrdiff_t last;
    uint32_t max;
    int visited;
    int steps;
}ind;

ind *createNodeIndex(uint32_t);

int lookup(ind*, uint32_t, uint32_t);

ptrdiff_t insertNode(ind**, uint32_t, list_node**, uint32_t*, uint32_t*, ptrdiff_t*);

int reallocNodeIndex(ind**, int, uint32_t*);

ptrdiff_t addEdge(ind**, uint32_t, uint32_t, list_node**, uint32_t*, ptrdiff_t*);

ptrdiff_t getListHead(ind*, uint32_t);

int destroyNodeIndex(ind*);
