#include "buffer.h"

#define IND_SIZE 16

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

extern uint32_t index_size;


unsigned long* createNodeIndex();
int insertNode(unsigned long*, uint32_t, uint32_t, list_node*);
unsigned long getListHead(unsigned long*, uint32_t);
int destroyNodeIndex(unsigned long*);
int reallocNodeIndex(unsigned long*, int);
int checkIfExists(unsigned long*, uint32_t);
int addEdge(unsigned long*, uint32_t, uint32_t, list_node*);

