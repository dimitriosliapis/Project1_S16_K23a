#include "index.h"

ind *createNodeIndex(uint32_t index_size) {

    ind *index = NULL;
    int i = 0;

    index = malloc(sizeof(ind) * index_size);
    for (i = 0; i < index_size; i++) {
        index[i].first = -1;
        index[i].last = -1;
        index[i].max = 0;
        index[i].neighbors = NULL;
    }

    return index;
}

int lookup(ind *index, uint32_t id, uint32_t index_size) {

    if (id >= index_size) return NOT_EXIST; // an den to xwraei den uparxei
    if (index[id].first != -1) return ALR_EXISTS;
    return NOT_EXIST;
}

ptrdiff_t insertNode(ind **index, uint32_t id, list_node **buffer, uint32_t *index_size, uint32_t *buffer_size, ptrdiff_t *available) {

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

int reallocNodeIndex(ind **index, int id, uint32_t *index_size) {

    uint32_t realloc_size = *index_size;
    uint32_t i = 0;
    ind *new = NULL;

    while (id >= realloc_size) realloc_size = realloc_size * 2; // diplasiasmos tou size mexri na xwraei to id
    new = realloc(*index, realloc_size * sizeof(ind));
    *index = new;

    for (i = *index_size; i < realloc_size; i++) {  // arxikopoihsh twn newn index nodes
        (*index)[i].first = -1;
        (*index)[i].last = -1;
        (*index)[i].max = DEFAULT;
        (*index)[i].neighbors = NULL;
    }
    *index_size = realloc_size;
    return OK_SUCCESS;
}

ptrdiff_t addEdge(ind **index, uint32_t id, uint32_t neighbor, list_node **buffer, uint32_t *buffer_size, ptrdiff_t *available) {

    int i = 0;
    ptrdiff_t offset = 0, prev = 0;

    offset = getListHead(*index, id);   // offset 1ou komvou sto buffer gia to id
    list_node *current = *buffer + (*index)[id].last;

//    if (neighbor > (*index)[id].max) {
//        current = *buffer + (*index)[id].last;
//        (*index)[id].max = neighbor;
//    }

    if ((*index)[id].neighbors == NULL)
        (*index)[id].neighbors = createHashtable(HT_SMALL);

    if (search((*index)[id].neighbors, neighbor, HT_SMALL) == FOUND)
        return ALR_CONNECTED;
    else
        insert((*index)[id].neighbors, neighbor, HT_SMALL);

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
    return offset;
}

ptrdiff_t getListHead(ind *index, uint32_t id) {

    if (index == NULL) return -1;
    return index[id].first;
}

int destroyNodeIndex(ind *index, uint32_t index_size) {

    int i = 0;

    if (index == NULL) return IND_EMPTY;

    for (i = 0; i < index_size; i++)
        delete(index[i].neighbors, HT_SMALL);

    free(index);

    return OK_SUCCESS;
}
