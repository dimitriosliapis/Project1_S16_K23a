#include "search.h"

#define INIT_UNODE_SIZE 32
#define INIT_NEWNODE_SIZE 8
#define METRIC 1250
#define STACK_ARRAY_SIZE 2048

/*typedef struct stack {
    int *stack_array;
    int size;
    int first;
    int last;
    int count;
} Stack_t;

Stack_t *createStack();
int stackisempty(Stack_t *stack);
void pushinstack(Stack_t *stack, int id);
int popfromstack(Stack_t *stack);
int peekfromstack(Stack_t *stack);
void deletestack(Stack_t *stack);*/

typedef struct sNode_t{
    int id;
    struct sNode_t *next;
}sNode;

typedef struct stack_t{
    sNode *last;
}Stack;

typedef struct u_unode_t{
    int *cc_array;// pinakas me CC pou sindeetai
    int size;//megethos cc_array
    int *new_nodes;//pinakas me kanourious komvous pou tha prostethoun se auto to CC kai den uphrxan prin se kanena
    int n_size;//megethos new_nodes
    char state;//o -> old, e -> empty, n -> new
}u_node;


void push(Stack*, int);
int pop(Stack*);
int stackIsEmpty(Stack*);
void deleteStack(Stack*);

typedef struct CC_t{
    int *cc_index;
    int cc_size; //megethos cc_index
    int cc_max;
    u_node * updateIndex;
    int u_size;//megethos updateIndex
    int metricVal;
}CC;

CC* createCCIndex(int, ind*, ind*, list_node*, list_node*, int, int, ht_Node*, int, int);
void initUpdateIndex(CC *);
void refreshUpdateIndex(CC *, int, int);
int searchUpdateIndex(CC, int, int, ht_Node*, int, int);
int updateCCIndex(CC *, ht_Node*, int, int, int);
void destroyCCIndex(CC*);
