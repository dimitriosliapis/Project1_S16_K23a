#include "search.h"

#define INIT_UNODE_SIZE 32
#define INIT_NEWNODE_SIZE 8
#define METRIC 1250
#define STACK_ARRAY_SIZE 2048

typedef struct stack {
    uint32_t *stack_array;
    int size;
    int first;
    int last;
    int count;
} Stack_t;

Stack_t *createStack();
int stackisempty(Stack_t *stack);
void pushinstack(Stack_t *stack, uint32_t id);
uint32_t popfromstack(Stack_t *stack);
uint32_t peekfromstack(Stack_t *stack);
void deletestack(Stack_t *stack);

/*typedef struct sNode_t{
    uint32_t id;
    struct sNode_t *next;
}sNode;*/

/*typedef struct stack_t{
    sNode *last;
}Stack;*/

typedef struct u_unode_t{
    uint32_t *cc_array;// pinakas me CC pou sindeetai
    uint32_t size;//megethos cc_array
    uint32_t *new_nodes;//pinakas me kanourious komvous pou tha prostethoun se auto to CC kai den uphrxan prin se kanena
    uint32_t n_size;//megethos new_nodes
    char state;//o -> old, e -> empty, n -> new
}u_node;


/*void push(Stack*, uint32_t);
uint32_t pop(Stack*);
int stackIsEmpty(Stack*);
void deleteStack(Stack*);*/

typedef struct CC_t{
    uint32_t *cc_index;
    uint32_t cc_size; //megethos cc_index
    uint32_t cc_max;
    u_node * updateIndex;
    uint32_t u_size;//megethos updateIndex
    uint32_t metricVal;
}CC;

CC* createCCIndex(uint32_t, ind*, ind*, list_node*, list_node*, uint32_t, uint32_t, ht_Node*, uint32_t, int);
void initUpdateIndex(CC *);
void refreshUpdateIndex(CC *, uint32_t, uint32_t);
int searchUpdateIndex(CC, uint32_t, uint32_t, ht_Node*, uint32_t, int);
uint32_t updateCCIndex(CC *, ht_Node*, uint32_t, uint32_t, int);
void destroyCCIndex(CC*);
