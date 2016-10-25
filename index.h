#include "buffer.h"

#define IND_SIZE 16

#define IND_EMPTY -1
#define ALLOC_FAIL -2
#define ALR_EXISTS -23
#define NOT_EXIST -24
#define ALR_CONNECTED -25

extern uint32_t index_size;

//typedef list_node** NodeIndex;

list_node** createNodeIndex();
int insertNode(list_node*[], uint32_t, uint32_t, list_node*);
list_node* getListHead(list_node*[], uint32_t);
int destroyNodeIndex(list_node*[]);
int reallocNodeIndex(list_node**, int);