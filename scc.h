#ifndef SCC_H
#define SCC_H

#include "cc.h"

#define COMPONENT_SIZE 270000
#define NODE_IDS_SIZE 16

typedef struct Component_t {
    uint32_t included_nodes_count; //number of nodes in component
    uint32_t *included_node_ids;    //pinakas me ta IDs twn komvwn pou periexei to SCC
    uint32_t node_array_size;
} Component;

typedef struct SCC_t {
    Component *components; // Components index - a vector which stores the components information
    uint32_t components_count;
    uint32_t component_size;
    uint32_t *id_belongs_to_component; //inverted index
} SCC;

SCC *tarjanRecursive(SCC **scc, ind *index_out, list_node *buffer_out, uint32_t num_nodes, uint32_t version, uint32_t v,
                     uint32_t *index, Stack_t *scc_stack);

SCC *estimateStronglyConnectedComponents(ind *, list_node *, uint32_t, uint32_t);

SCC *estimateStronglyConnectedComponents_iterative(ind *index_out, list_node *buffer_out, uint32_t num_nodes, uint32_t version);

void tarjan_iterative(SCC **scc, ind *index_out, list_node *buffer_out, uint32_t v, uint32_t index, Stack_t *scc_stack, uint32_t *neigh_counter, uint32_t *caller, uint32_t *list_node_counter);

void destroyStronglyConnectedComponents(SCC *);

#endif
