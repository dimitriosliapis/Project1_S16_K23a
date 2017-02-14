#ifndef SCC_H
#define SCC_H

#include "cc.h"

#define COMPONENT_SIZE 270000
#define NODE_IDS_SIZE 16

typedef struct Component_t {
    uint32_t included_nodes_count;
    uint32_t *included_node_ids;
    uint32_t node_array_size;
} Component;

typedef struct SCC_t {
    Component *components;
    uint32_t components_count;
    uint32_t component_size;
    uint32_t *id_belongs_to_component;
} SCC;

SCC *estimateStronglyConnectedComponents_iterative(ind *, list_node *, uint32_t, uint32_t);

void tarjan_iterative(SCC **, ind *, list_node *, uint32_t, uint32_t *, Stack_t *, uint32_t *, uint32_t);

void destroyStronglyConnectedComponents(SCC *);

#endif
