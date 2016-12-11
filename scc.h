#include "cc.h"

#define COMPONENT_SIZE 270000
#define NODE_IDS_SIZE 16
#define STACK_ARRAY_SIZE 2048

typedef struct stack {
    uint32_t *stack_array;
    int size;
    int first;
    int last;
    int count;
} Stack_t;

typedef struct Component_t{
    uint32_t included_nodes_count; //number of nodes in component
    uint32_t *included_node_ids;
    uint32_t node_array_size;
}Component;

typedef struct SCC_t{
    Component *components; // Components index - a vector which stores the components information
    uint32_t components_count;
    uint32_t component_size;
    uint32_t *id_belongs_to_component; //inverted index
}SCC;

uint32_t peek(Stack *);
//SCC* tarjan(ind*, list_node*, uint32_t, uint32_t, ht_Node*, ht_Node*, ht_Node*, uint32_t);
SCC* tarjanRecursive(SCC **scc, ind *index_out, list_node *buffer_out, uint32_t num_nodes, uint32_t version, uint32_t v, uint32_t *index, Stack *scc_stack);


SCC* estimateStronglyConnectedComponents(ind *, list_node *, uint32_t, uint32_t);
void destroyStronglyConnectedComponents(SCC*);
