#include "buffer.h"

#define IND_SIZE 16

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

extern uint32_t index_size;


ptrdiff_t* createNodeIndex();
int insertNode(ptrdiff_t*, uint32_t, uint32_t, list_node*);
ptrdiff_t getListHead(ptrdiff_t*, uint32_t);
int destroyNodeIndex(ptrdiff_t*);
int reallocNodeIndex(ptrdiff_t*, int);
int checkIfExists(ptrdiff_t*, uint32_t);
int addEdge(ptrdiff_t*, uint32_t, uint32_t, list_node*);

