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
    int visited[THREAD_POOL_SIZE+1];
    int lowlink;
    int index;
    int rank;
    int min_rank;
    ht_Node *neighbors;
    int all_children_in_scc;
    int num_of_children;
    int onStack;
}ind;

ind *createNodeIndex(int);

int lookup(ind*, int, int);

ptrdiff_t insertNode(ind**, int, list_node**, int*, int*, ptrdiff_t*);

int reallocNodeIndex(ind**, int, int*);

ptrdiff_t addEdge(ind**, int, int, list_node**, int*, ptrdiff_t*, int);

ptrdiff_t getListHead(ind*, int);

int freeNeighbors(ind *index, int index_size);

int destroyNodeIndex(ind*, int index_size);

