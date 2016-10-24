#include "buffer.h"

#define IND_SIZE 16

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23

extern uint32_t index_size;

typedef list_node** NodeIndex;

NodeIndex createNodeIndex();
int insertNode(NodeIndex*, uint32_t, uint32_t*, Buffer*);
list_node* getListHead(NodeIndex*, uint32_t);
int destroyNodeIndex(NodeIndex*);