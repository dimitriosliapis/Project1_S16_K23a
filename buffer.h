#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define BUFF_SIZE 262144
#define N 16
#define DEFAULT 16777216

#define OK_SUCCESS 0
#define ERROR -1

typedef struct list_node_t {
    int neighbor[N];
    int edgeProperty[N];
    ptrdiff_t nextListNode;
}list_node;

list_node *createBuffer(int);

ptrdiff_t allocNewNode(list_node**, int*, ptrdiff_t);

int reallocBuffer(list_node**, int*);

int destroyBuffer(list_node*);
