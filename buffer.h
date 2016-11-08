#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define BUFF_SIZE 4096
#define N 256
#define DEFAULT 16777216

#define OK_SUCCESS 0;
#define ERROR -1

typedef struct list_node_t {
    uint32_t neighbor[N];
    //uint32_t edgeProperty[N];
    ptrdiff_t nextListNode;
} list_node;

list_node *createBuffer(uint32_t);

ptrdiff_t allocNewNode(list_node**, uint32_t*, ptrdiff_t);

int reallocBuffer(list_node**, uint32_t*);

int destroyBuffer(list_node*);
