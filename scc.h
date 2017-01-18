#include "cc.h"

#define COMPONENT_SIZE 270000
#define NODE_IDS_SIZE 16


typedef struct Component_t{
    int included_nodes_count; //number of nodes in component
    int *included_node_ids;    //pinakas me ta IDs twn komvwn pou periexei to SCC
    int node_array_size;
}Component;

typedef struct SCC_t{
    Component *components; // Components index - a vector which stores the components information
    int components_count;
    int component_size;
    int *id_belongs_to_component; //inverted index
}SCC;

int peek(Stack *);
//SCC* tarjan(ind*, list_node*, int, int, ht_Node*, ht_Node*, ht_Node*, int);
SCC* tarjanRecursive(SCC **scc, ind *index_out, list_node *buffer_out, int num_nodes, int version, int v, int *index, Stack *scc_stack, int);


SCC* estimateStronglyConnectedComponents(ind *, list_node *, int, int, int);

int scc_bBFS(ind *, ind *, list_node *, list_node *, SCC *, int, int, Queue *, Queue *, int, int);
void destroyStronglyConnectedComponents(SCC*);
