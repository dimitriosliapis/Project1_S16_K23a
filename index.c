#include <stdint.h>
#include "index.h"

ind *createNodeIndex(int index_size) {

    ind *index = NULL;
    int i = 0;
    int j = 0;

    index = malloc(sizeof(ind) * index_size);
    for (i = 0; i < index_size; i++) {
        index[i].first = -1;
        index[i].last = -1;
/*        for(j = 0; j < THREAD_POOL_SIZE; j++) {
            index[i].visited[j] = DEFAULT;
        }*/
        index[i].lowlink = DEFAULT;
        index[i].index = DEFAULT;
        index[i].neighbors = NULL;
        index[i].all_children_in_scc = 0;
        index[i].rank = DEFAULT;
        index[i].min_rank = DEFAULT;
        index[i].num_of_children = 0;
    }

    return index;
}

int lookup(ind *index, int id, int index_size) {

    if (id >= index_size) return NOT_EXIST; // an den to xwraei den uparxei
    if (index[id].first != -1) return ALR_EXISTS;
    return NOT_EXIST;
}

ptrdiff_t insertNode(ind **index, int id, list_node **buffer, int *index_size, int *buffer_size, ptrdiff_t *available) {

    ptrdiff_t offset = 0;

    if (index == NULL) return IND_EMPTY;

    offset = allocNewNode(&(*buffer), &(*buffer_size), *available); // to offset tou prwtou eleutherou komvou sto buffer
    if (offset == -1) return ALLOC_FAIL;

    if (id >= *index_size) {
        reallocNodeIndex(&(*index), id, &(*index_size));    // an den ton xwraei to index realloc
    }

    (*index)[id].first = offset;    // tuxaia seira sto buffer etsi
    (*index)[id].last = offset; // tuxaia seira sto buffer etsi

    (*available)++;

    return offset;
}

int reallocNodeIndex(ind **index, int id, int *index_size) {

    int realloc_size = *index_size;
    int i = 0;
    int j = 0;
    ind *new = NULL;

    while (id >= realloc_size) realloc_size = realloc_size * 2; // diplasiasmos tou size mexri na xwraei to id
    new = realloc(*index, realloc_size * sizeof(ind));
    *index = new;

    for (i = *index_size; i < realloc_size; i++) {  // arxikopoihsh twn newn index nodes
        (*index)[i].first = -1;
        (*index)[i].last = -1;
        /*for(j = 0; j < THREAD_POOL_SIZE; j++) {
            (*index)[i].visited[j] = DEFAULT;
        }*/
        (*index)[i].lowlink = DEFAULT;
        (*index)[i].index = DEFAULT;
        (*index)[i].neighbors = NULL;
        (*index)[i].all_children_in_scc = 0;
        (*index)[i].rank = DEFAULT;
        (*index)[i].min_rank = DEFAULT;
        (*index)[i].num_of_children = 0;

    }
    *index_size = realloc_size;
    return OK_SUCCESS;
}

ptrdiff_t addEdge(ind **index, int id, int neighbor, list_node **buffer, int *buffer_size, ptrdiff_t *available, int thread_id) {

    int i = 0;
    int version = 0;
    ptrdiff_t offset = 0, prev = 0;

    offset = getListHead(*index, id);   // offset 1ou komvou sto buffer gia to id
    list_node *current = *buffer + (*index)[id].last;

    if ((*index)[id].neighbors == NULL) {
        (*index)[id].neighbors = createHashtable(HT_SMALL);
        insert((*index)[id].neighbors, neighbor, HT_SMALL, version, thread_id);
    }
    else {
        if (search((*index)[id].neighbors, neighbor, HT_SMALL, version, thread_id) == FOUND)
            return ALR_CONNECTED;
        else
            insert((*index)[id].neighbors, neighbor, HT_SMALL, version, thread_id);
    }
    while (i < N) { // psaxnei stous geitones (max N ana komvo)
//        if (current->neighbor[i] == neighbor) return ALR_CONNECTED; // gia na dei an uparxei
        if (current->neighbor[i] == DEFAULT) {  // alliws vriskei tin thesi tou 1ou diathesimou
            current->neighbor[i] = neighbor;    // kai ton vazei ekei
            break;
        }
        i++;
        if (i == N) {   // an ftasei to N paei ston epomeno komvo
            if (current->nextListNode == -1) {  // an den uparxei ton dimiourgei
                prev = current - *buffer;
                offset = allocNewNode(&(*buffer), &(*buffer_size), *available);
                (*available)++;
                current = *buffer + prev;
                current->nextListNode = offset;
                current = *buffer + offset;
                (*index)[id].last = offset;
            } else
                current = *buffer + current->nextListNode;  // an uparxei sunexizei se auton
            i = 0;
        }
    }
    (*index)[id].num_of_children++;
    return offset;
}

ptrdiff_t getListHead(ind *index, int id) {

    if (index == NULL) return -1;
    return index[id].first;
}

int freeNeighbors(ind *index, int index_size) {

    int i;

    for (i = 0; i < index_size; i++) {
        if(lookup(index, i, index_size) == NOT_EXIST) continue;
        if (index[i].neighbors != NULL) delete(index[i].neighbors, HT_SMALL);
        index[i].neighbors = NULL;
    }
    return OK_SUCCESS;
}

int destroyNodeIndex(ind *index, int index_size) {

    int i = 0;

    if (index == NULL) return IND_EMPTY;

    for (i = 0; i < index_size; i++)
        if(index[i].neighbors != NULL) delete(index[i].neighbors, HT_SMALL);

    free(index);

    return OK_SUCCESS;
}