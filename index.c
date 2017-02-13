#include <stdint.h>
#include "index.h"
#include "grail.h"

ind *createNodeIndex(uint32_t index_size, int s) {

    ind *index = NULL;
    int i = 0, j = 0;

    index = malloc(sizeof(ind) * index_size);

    if (s) {
        for (i = 0; i < index_size; i++) {
            index[i].first = -1;
            index[i].last = -1;
            for (j = 0; j < THREAD_POOL_SIZE; j++)
                index[i].visited[j] = -1;

            index[i].neighbors = NULL;

            index[i].num_of_children = 0;
            index[i].s_data = malloc(sizeof(static_data));
            index[i].s_data->lowlink = DEFAULT;
            index[i].s_data->index = DEFAULT;
            index[i].s_data->all_children_in_scc = 0;
            index[i].s_data->rank = malloc(NUM_GRAIL * sizeof(uint32_t));
            index[i].s_data->min_rank = malloc(NUM_GRAIL * sizeof(uint32_t));
            index[i].s_data->onStack = 0;
            index[i].s_data->children_in_scc = 0;
            index[i].s_data->curr_neighbors = NULL;
            index[i].s_data->next_child = NULL;
            index[i].s_data->n = 0;
        }

    } else {
        for (i = 0; i < index_size; i++) {
            index[i].first = -1;
            index[i].last = -1;
            for (j = 0; j < THREAD_POOL_SIZE; j++)
                index[i].visited[j] = -1;

            index[i].neighbors = NULL;

            index[i].num_of_children = 0;

        }
    }

    return index;
}

int lookup(ind *index, uint32_t id, uint32_t index_size) {

    return ((id >= index_size) || (index[id].first == -1) ? NOT_EXIST : ALR_EXISTS); // an den to xwraei den uparxei
}

ptrdiff_t insertNode(ind **index, uint32_t id, list_node **buffer, uint32_t *index_size, uint32_t *buffer_size,
                     ptrdiff_t *available, int s) {

    ptrdiff_t offset = 0;

    if (index == NULL) return IND_EMPTY;

    offset = allocNewNode(&(*buffer), &(*buffer_size), *available); // to offset tou prwtou eleutherou komvou sto buffer
    if (offset == -1) return ALLOC_FAIL;

    if (id >= *index_size)
        reallocNodeIndex(&(*index), id, &(*index_size), s);    // an den ton xwraei to index realloc

    (*index)[id].first = offset;    // tuxaia seira sto buffer etsi
    (*index)[id].last = offset; // tuxaia seira sto buffer etsi

    (*available)++;

    return offset;
}

int reallocNodeIndex(ind **index, int id, uint32_t *index_size, int s) {

    uint32_t realloc_size = *index_size;
    uint32_t i = 0, j = 0;
    ind *new = NULL;

    while (id >= realloc_size) realloc_size = realloc_size * 2; // diplasiasmos tou size mexri na xwraei to id
    new = realloc(*index, realloc_size * sizeof(ind));
    *index = new;


    if (s) {
        for (i = *index_size; i < realloc_size; i++) {
            (*index)[i].first = -1;
            (*index)[i].last = -1;
            for (j = 0; j < THREAD_POOL_SIZE; j++)
                (*index)[i].visited[j] = -1;
            (*index)[i].neighbors = NULL;
            (*index)[i].num_of_children = 0;

            (*index)[i].s_data = malloc(sizeof(static_data));
            (*index)[i].s_data->lowlink = DEFAULT;
            (*index)[i].s_data->index = DEFAULT;
            (*index)[i].s_data->all_children_in_scc = 0;
            (*index)[i].s_data->rank = malloc(NUM_GRAIL * sizeof(uint32_t));;
            (*index)[i].s_data->min_rank = malloc(NUM_GRAIL * sizeof(uint32_t));;
            (*index)[i].s_data->onStack = 0;
            (*index)[i].s_data->children_in_scc = 0;
            (*index)[i].s_data->curr_neighbors = NULL;
            (*index)[i].s_data->next_child = NULL;
            (*index)[i].s_data->n = 0;
        }

    } else {
        for (i = *index_size; i < realloc_size; i++) {  // arxikopoihsh twn newn index nodes
            (*index)[i].first = -1;
            (*index)[i].last = -1;
            for (j = 0; j < THREAD_POOL_SIZE; j++)
                (*index)[i].visited[j] = -1;
            (*index)[i].neighbors = NULL;
            (*index)[i].num_of_children = 0;

        }
    }

    *index_size = realloc_size;
    return OK_SUCCESS;
}

ptrdiff_t
addEdge(ind **index, uint32_t id, uint32_t neighbor, list_node **buffer, uint32_t *buffer_size, ptrdiff_t *available,
        int check, uint32_t edge_version) {

    int i = 0;
    uint32_t version = 0;
    ptrdiff_t offset = 0, prev = 0;

    offset = getListHead(*index, id);   // offset 1ou komvou sto buffer gia to id
    list_node *current = *buffer + (*index)[id].last;

    if (check == 1) {
        if ((*index)[id].neighbors == NULL) {
            (*index)[id].neighbors = createHashtable(HT_SMALL);
            insert((*index)[id].neighbors, neighbor, HT_SMALL, version);
        } else {
            if (search((*index)[id].neighbors, neighbor, HT_SMALL, version) == FOUND)
                return ALR_CONNECTED;
            else
                insert((*index)[id].neighbors, neighbor, HT_SMALL, version);
        }
    }

    //while(current->nextListNode != -1) current = *buffer + current->nextListNode;  // an uparxei sunexizei se auton

    while (i < N) { // psaxnei stous geitones (max N ana komvo)
        if (current->neighbor[i] == DEFAULT) {  // alliws vriskei tin thesi tou 1ou diathesimou
            current->neighbor[i] = neighbor;    // kai ton vazei ekei
            current->edgeProperty[i] = edge_version;
            break;
        }
        i++;
        if (i == N) {   // an ftasei to N paei ston epomeno komvo
            //if (current->nextListNode == -1) {  // an den uparxei ton dimiourgei
            prev = current - *buffer;
            offset = allocNewNode(&(*buffer), &(*buffer_size), *available);
            (*available)++;
            current = *buffer + prev;
            current->nextListNode = offset;
            current = *buffer + offset;
            (*index)[id].last = offset;
            //} else
            //  current = *buffer + current->nextListNode;  // an uparxei sunexizei se auton
            i = 0;
        }
    }
    (*index)[id].num_of_children++;
    return offset;
}

ptrdiff_t getListHead(ind *index, uint32_t id) {

    return (index == NULL ? -1 : index[id].first);
}

int destroyNodeIndex(ind *index, uint32_t index_size, int s) {

    int i = 0;

    if (index == NULL) return IND_EMPTY;

    if (s == 1) {
        for (i = 0; i < index_size; i++) {
            delete(index[i].neighbors, HT_SMALL);
            free(index[i].s_data->min_rank);
            free(index[i].s_data->rank);
            free(index[i].s_data);
        }
    } else if (s == 0) {
        for (i = 0; i < index_size; i++) {
            delete(index[i].neighbors, HT_SMALL);
        }
    } else if (s == 2) {
        for (i = 0; i < index_size; i++) {
            free(index[i].s_data->rank);
            free(index[i].s_data->min_rank);
            free(index[i].s_data);
        }
    }
    free(index);

    return OK_SUCCESS;
}
