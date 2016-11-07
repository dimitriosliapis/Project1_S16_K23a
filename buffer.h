#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 8192
#define N 128
#define DEFAULT 16777216

#define OK_SUCCESS 0;
#define ERROR -1


typedef struct list_node_t {
    uint32_t neighbor[N]; //the ids of the neighbor nodes
    //uint32_t edgeProperty[N]; //property for each edge
    ptrdiff_t nextListNode;
} list_node;

list_node *createBuffer(uint32_t);

ptrdiff_t allocNewNode(list_node**, uint32_t*, ptrdiff_t);

list_node *getListNode(list_node*);

int destroyBuffer(list_node*);

int reallocBuffer(list_node**, uint32_t*);
