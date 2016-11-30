#include "cc.h"

#define COMPONENT_SIZE 32
#define NODE_IDS_SIZE 16


typedef struct Component_t{
    uint32_t component_id; //current component id
    uint32_t included_nodes_count; //number of nodes in component
    uint32_t *included_node_ids;
}Component;

typedef struct SCC_t{
    Component *components; // Components index - a vector which stores the components information
    uint32_t components_count;
    uint32_t *id_belongs_to_component; //inverted index

}SCC;

uint32_t peek(Stack *);
void tarjan(ind*, list_node*, uint32_t);