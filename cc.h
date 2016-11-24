

typedef struct sNode_t{
    uint32_t id;
    struct sNode_t *next;
}sNode;

typedef struct stack_t{
    sNode *first;
}Stack;



void push(Stack*, uint32_t);
uint32_t pop(Stack*);
void deleteStack(Stack*, sNode*);


struct CC{
    uint32_t ccindex[]; //CCIndex
    UpdateIndex* updateIndex;
    uint32_t metricVal;
};