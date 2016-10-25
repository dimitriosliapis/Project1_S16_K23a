#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <stddef.h>


#define BUFF_SIZE 32
#define N 8
#define DEFAULT 16777216

#define OK_SUCCESS 0;
#define ERROR -1

extern uint32_t buffer_size;

typedef struct list_node_t{
    uint32_t neighbor[N]; //the ids of the neighbor nodes
    //uint32_t edgeProperty[N]; //property for each edge
    char empty;
    struct list_node_t *nextListNode;
}list_node;


list_node* createBuffer();
ptrdiff_t allocNewNode(list_node *b);
ptrdiff_t getListNode(list_node*);
int destroyBuffer(list_node*);
int reallocBuffer(list_node*);