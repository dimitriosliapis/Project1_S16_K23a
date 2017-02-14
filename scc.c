#include "scc.h"

// Estimate strongly connected components
SCC *estimateStronglyConnectedComponents_iterative(ind *index_out, list_node *buffer_out, uint32_t num_nodes,
                                                   uint32_t max) {

    Stack_t *scc_stack = createStack();
    uint32_t i, index;

    SCC *scc = malloc(sizeof(SCC));

    scc->components = malloc(COMPONENT_SIZE * sizeof(Component));
    scc->component_size = COMPONENT_SIZE;

    for (i = 0; i < scc->component_size; i++) {
        scc->components[i].included_node_ids = NULL;
        scc->components[i].included_nodes_count = 0;
        scc->components[i].node_array_size = 0;
    }

    scc->components_count = 0;

    scc->id_belongs_to_component = malloc(max * sizeof(uint32_t));
    uint32_t *caller = malloc(max * sizeof(uint32_t));

    for (i = 0; i < max; i++) {
        scc->id_belongs_to_component[i] = DEFAULT;
        caller[i] = DEFAULT;
    }

    index = 1;

    for (i = 0; i < num_nodes; i++) {

        if (lookup(index_out, i, num_nodes) == NOT_EXIST) continue;
        if (scc->id_belongs_to_component[i] != DEFAULT) continue;

        tarjan_iterative(&scc, index_out, buffer_out, i, &index, scc_stack, caller, num_nodes);
    }

    deletestack(scc_stack);

    free(caller);

    return scc;
}

// Tarjan algorithm
void tarjan_iterative(SCC **scc, ind *index_out, list_node *buffer_out, uint32_t v, uint32_t *index, Stack_t *scc_stack,
                      uint32_t *caller, uint32_t num_nodes) {

    uint32_t w, last, new_last, scc_counter, i, realloc_node_size, k = 0;
    ptrdiff_t offset_out;
    list_node *neighbors_out;

    index_out[v].s_data->index = *index;
    index_out[v].s_data->lowlink = *index;
    index_out[v].s_data->children_in_scc = 0;
    offset_out = getListHead(index_out, v);
    index_out[v].s_data->curr_neighbors = buffer_out + offset_out;
    index_out[v].s_data->next_child = index_out[v].s_data->curr_neighbors->neighbor;
    last = v;
    pushinstack(scc_stack, v);
    index_out[v].s_data->onStack = 1;

    (*index)++;

    while (1) {

        if (index_out[last].s_data->children_in_scc < index_out[last].num_of_children) {

            w = *index_out[last].s_data->next_child;
            index_out[last].s_data->children_in_scc++;

            index_out[last].s_data->n++;
            if (index_out[last].s_data->n == N) {
                if (index_out[last].s_data->curr_neighbors->nextListNode != -1) {
                    index_out[last].s_data->curr_neighbors =
                            buffer_out + index_out[last].s_data->curr_neighbors->nextListNode;
                    index_out[last].s_data->next_child = index_out[last].s_data->curr_neighbors->neighbor;
                    index_out[last].s_data->n = 0;
                } else
                    index_out[last].s_data->next_child = NULL;
            } else
                index_out[last].s_data->next_child++;

            if (w == DEFAULT) break;

            if (lookup(index_out, w, num_nodes) == NOT_EXIST) {
                if ((*scc)->id_belongs_to_component[w] == DEFAULT) {

                    scc_counter = (*scc)->components_count;

                    if (scc_counter == (*scc)->component_size) {
                        (*scc)->component_size *= 2;
                        (*scc)->components = realloc((*scc)->components, (*scc)->component_size * sizeof(Component));
                        for (i = scc_counter; i < (*scc)->component_size; i++) {
                            (*scc)->components[i].included_node_ids = NULL;
                            (*scc)->components[i].included_nodes_count = 0;
                            (*scc)->components[i].node_array_size = 0;
                        }
                    }

                    (*scc)->components[scc_counter].included_nodes_count = 1;
                    (*scc)->components[scc_counter].node_array_size = 1;
                    (*scc)->components[scc_counter].included_node_ids = malloc(sizeof(uint32_t));

                    (*scc)->components[scc_counter].included_node_ids[0] = w;
                    (*scc)->id_belongs_to_component[w] = scc_counter;

                    (*scc)->components_count++;
                }

            } else {

                if (index_out[w].s_data->index == DEFAULT) {    // if w is unvisited

                    caller[w] = last;
                    index_out[w].s_data->index = *index;
                    index_out[w].s_data->lowlink = *index;
                    index_out[w].s_data->children_in_scc = 0;
                    offset_out = getListHead(index_out, w);
                    index_out[w].s_data->curr_neighbors = buffer_out + offset_out;
                    index_out[w].s_data->next_child = index_out[w].s_data->curr_neighbors->neighbor;
                    (*index)++;

                    if (index_out[w].s_data->onStack == 0) {
                        pushinstack(scc_stack, w);
                        index_out[w].s_data->onStack = 1;
                        last = w;
                    }
                } else if (index_out[w].s_data->onStack == 1) {
                    if (index_out[last].s_data->lowlink > index_out[w].s_data->index)
                        index_out[last].s_data->lowlink = index_out[w].s_data->index;
                }
            }

        } else {
            if (index_out[last].s_data->lowlink == index_out[last].s_data->index) {

                scc_counter = (*scc)->components_count;

                if (scc_counter == (*scc)->component_size) {
                    (*scc)->component_size *= 2;
                    (*scc)->components = realloc((*scc)->components, (*scc)->component_size * sizeof(Component));
                    for (i = scc_counter; i < (*scc)->component_size; i++) {
                        (*scc)->components[i].included_node_ids = NULL;
                        (*scc)->components[i].included_nodes_count = 0;
                        (*scc)->components[i].node_array_size = 0;
                    }
                }

                (*scc)->components[scc_counter].included_nodes_count = 0;
                (*scc)->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                (*scc)->components[scc_counter].included_node_ids = malloc(NODE_IDS_SIZE * sizeof(uint32_t));
                (*scc)->components[scc_counter].included_node_ids[0] = DEFAULT;

                i = 0;
                do {    // build SCC

                    w = popfromstack(scc_stack);
                    index_out[w].s_data->onStack = 0;

                    if (i == ((*scc)->components[scc_counter].node_array_size)) {
                        realloc_node_size = 2 * (*scc)->components[scc_counter].node_array_size;
                        (*scc)->components[scc_counter].included_node_ids = realloc(
                                (*scc)->components[scc_counter].included_node_ids,
                                realloc_node_size * sizeof(uint32_t));
                        (*scc)->components[scc_counter].node_array_size = realloc_node_size;
                    }

                    (*scc)->components[scc_counter].included_node_ids[i] = w;
                    (*scc)->components[scc_counter].included_nodes_count++;
                    (*scc)->id_belongs_to_component[w] = scc_counter;

                    i++;

                } while (w != last);

                (*scc)->components_count++;
            }

            new_last = caller[last];

            if (new_last != DEFAULT) {  // go up one recursive call
                if (index_out[new_last].s_data->lowlink > index_out[last].s_data->lowlink)
                    index_out[new_last].s_data->lowlink = index_out[last].s_data->lowlink;
                last = new_last;

            } else      // we've seen all the nodes
                break;
        }
    }

}

// Free SCC struct
void destroyStronglyConnectedComponents(SCC *scc) {

    free(scc->id_belongs_to_component);
    free(scc);
}
