#include "search.h"

#define CC_SIZE 4096

typedef struct sNode_t{
    uint32_t id;
    struct sNode_t *next;
}sNode;

typedef struct stack_t{
    sNode *last;
}Stack;



void push(Stack*, uint32_t);
uint32_t pop(Stack*);
int stackIsEmpty(Stack*);
void deleteStack(Stack*, sNode*);


typedef struct CC_t{
    uint32_t *ids; //CCInde
    uint32_t ** updateIndex;
    //uint32_t metricVal;*/
}CC;

uint32_t createCCIndex(uint32_t *, ind*, ind*, list_node*, list_node*, uint32_t, uint32_t);
int refreshUpdateIndex(uint32_t *, uint32_t **, uint32_t*, uint32_t*, uint32_t, uint32_t);
int searchUpdateIndex( uint32_t *,uint32_t **, uint32_t, uint32_t);
uint32_t findCCMax(uint32_t *, uint32_t);
int updateCCIndex(uint32_t *, ind *, ind *, list_node *, list_node *, uint32_t, uint32_t, uint32_t **, uint32_t *, uint32_t );