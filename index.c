#include "index.h"


ptrdiff_t *createNodeIndex(uint32_t index_size) {

    ptrdiff_t *index = NULL;
    int i = 0;

    index = malloc(sizeof(ptrdiff_t) * index_size);
    for (i = 0; i < index_size; i++) {
        index[i] = -1;
    }

    return index;
}

int lookup(ptrdiff_t *index, uint32_t id, uint32_t index_size) {

    if(id >= index_size) return NOT_EXIST;          //an den to xwraei den uparxei
    if (index[id] != -1) return ALR_EXISTS;
    else return NOT_EXIST;
}

ptrdiff_t insertNode(ptrdiff_t **index, uint32_t id, list_node **buffer, uint32_t *index_size, uint32_t *buffer_size, ptrdiff_t prev) {

    ptrdiff_t offset = 0;
    list_node *new = NULL;

    if (index == NULL) return IND_EMPTY;

    offset = allocNewNode(&(*buffer), &(*buffer_size), prev);         //to offset tou prwtou eleutherou komvou sto buffer
    if (offset == -1) return ALLOC_FAIL;

    if (id >= *index_size)
        reallocNodeIndex(index, id, &(*index_size));            //an den ton xwraei to index realloc

    (*index)[id] = offset;                                      //tuxaia seira sto buffer etsi

    new = *buffer + (*index)[id];
    new->empty = 'n';                                           //ton markarei

    return offset;
}

int reallocNodeIndex(ptrdiff_t **index, int id, uint32_t *index_size) {

    uint32_t realloc_size = *index_size;
    uint32_t i = 0;
    ptrdiff_t *new = NULL;

    while (id >= realloc_size) realloc_size = realloc_size * 2;    //Double size until id fits
    new = realloc(*index, realloc_size * sizeof(ptrdiff_t));
    *index = new;

    for (i = *index_size; i < realloc_size; i++) {    //Initialize new index nodes
        (*index)[i] = -1;
    }
    *index_size = realloc_size;

    return OK_SUCCESS;
}

ptrdiff_t addEdge(ptrdiff_t *index, uint32_t id, uint32_t neighbor, list_node **buffer, uint32_t *buffer_size, uint32_t index_size, ptrdiff_t prev) {

    int i = 0;
    ptrdiff_t offset = 0;

    offset = getListHead(index, id, index_size);                            //offset 1ou komvou sto buffer gia to id
    list_node *current = *buffer + offset;


    while (i < N) {                                                         //psaxnei stous geitones (max N ana komvo)

        if (current->neighbor[i] == neighbor) return ALR_CONNECTED;         //gia na dei an uparxei

        if (current->neighbor[i] == DEFAULT) {                              //alliws vriskei tin thesi tou 1ou diathesimou
            current->neighbor[i] = neighbor;                                //kai ton vazei ekei
            break;
        }

        i++;

        if (i == N) {                                                       //an ftasei to N paei ston epomeno komvo

            if (current->nextListNode == -1) {                              //an den uparxei ton dimiourgei
                offset = allocNewNode(&(*buffer), &(*buffer_size), prev);
                current->nextListNode = offset;
                current = *buffer + offset;
            } else
                current = *buffer + current->nextListNode;                  //an uparxei sunexizei se auton
            i = 0;
        }
    }

    return offset;
}

ptrdiff_t getListHead(ptrdiff_t *index, uint32_t id, uint32_t index_size) {

    if (index == NULL) return -1;
    if(id >= index_size) return -1;
    return index[id];
}

int destroyNodeIndex(ptrdiff_t *index) {

    if (index == NULL) return IND_EMPTY;
    free(index);
    return OK_SUCCESS;
}
