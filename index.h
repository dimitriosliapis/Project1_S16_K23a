#include "buffer.h"

#define IND_SIZE 128
#define BDFS_BUFF_SIZE 32

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

#define REALLOC_STRING -9


ptrdiff_t *createNodeIndex(uint32_t);

ptrdiff_t insertNode(ptrdiff_t**, uint32_t, list_node**, uint32_t*, uint32_t*, ptrdiff_t*);

ptrdiff_t getListHead(ptrdiff_t*, uint32_t, uint32_t);

int destroyNodeIndex(ptrdiff_t*);

int reallocNodeIndex(ptrdiff_t**, int, uint32_t*);

int lookup(ptrdiff_t*, uint32_t, uint32_t);

ptrdiff_t addEdge(ptrdiff_t*, uint32_t, uint32_t, list_node**, uint32_t*, uint32_t, ptrdiff_t*);

