#include <stdint.h>
#include <stdlib.h>


#define BUFF_SIZE 32
#define N 8
#define LIST_SIZE 16;
#define DEFAULT 16777216

#define OK_SUCCESS 0;
#define PTR_NOT_NULL -1
#define ERROR -1
#define BUFF_OUT_OF_SPACE -2

extern uint32_t buffer_size;

typedef struct list_node_t{
    uint32_t neighbor[N]; //the ids of the neighbor nodes
    //uint32_t edgeProperty[N]; //property for each edge
    char empty;
    struct list_node_t *nextListNode;
}list_node;

//typedef list_node* Buffer;

/*
 index index_in;
 index index_out;

 index_in = malloc(sizeof(list_node *) * INDEX_SIZE);
 index_in[i] = malloc(sizeof(list_node));
 index_out = malloc(sizeof(list_node *) * INDEX_SIZE);
 index_out[i] = malloc(sizeof(list_node));



 * */

list_node* createBuffer();
list_node* allocNewNode(list_node *b);
list_node* getListNode(list_node*);
int destroyBuffer(list_node*);