#include "index.h"


ptrdiff_t *createNodeIndex() {

    ptrdiff_t *index = NULL;
    int i = 0;

    index = malloc(sizeof(ptrdiff_t) * index_size);
    for (i = 0; i < index_size; i++) {
        index[i] = -1;
    }

    return index;
}

int lookup(ptrdiff_t *index, uint32_t id) {

    if (index[id] != -1) return ALR_EXISTS;
    return NOT_EXIST;
}

int insertNode(ptrdiff_t *index, uint32_t id, list_node *buffer) {

    ptrdiff_t offset = 0;
    list_node *new = NULL;

    if (index == NULL) return IND_EMPTY;

    offset = allocNewNode(buffer);

    if (id >= index_size) {
        reallocNodeIndex(index, id);
    }

    index[id] = offset;//tuxaia seira sto buffer etsi
    if (index[id] == -1) return ALLOC_FAIL;

    new = buffer + index[id];
    new->empty = 'n';

    return OK_SUCCESS;
}

int reallocNodeIndex(ptrdiff_t *index, int id) {

    uint32_t realloc_size = index_size;
    uint32_t a = 0;

    while (id >= realloc_size) realloc_size = realloc_size * 2;    //Double size until id fits
    index = realloc(index, realloc_size * sizeof(ptrdiff_t));

    for (a = index_size; a < realloc_size; a++) {    //Initialize new index nodes
        //Connect id index node to buffer -> tha ginei meta stin insertNode
        index[a] = -1;
    }
    index_size = realloc_size;
    return OK_SUCCESS;
}

int addEdge(ptrdiff_t *index, uint32_t id, uint32_t neighbor, list_node *buffer) {

    int i = 0;
    ptrdiff_t offset = 0;

    offset = getListHead(index, id);
    list_node *current = buffer + offset;

    while (i < N) {
        if (current->neighbor[i] == neighbor) return ALR_CONNECTED;
        if (current->neighbor[i] == DEFAULT) {
            current->neighbor[i] = neighbor;
            break;
        }
        i++;
        if (i == N) {
            if (current->nextListNode == -1) {
                offset = allocNewNode(buffer);
                current->nextListNode = offset;
               /* if (current->nextListNode == -1) {
                    reallocBuffer(buffer);
                    offset = allocNewNode(buffer);
                    current->nextListNode = offset;
                }*/
                current = buffer + current->nextListNode;
            } else
                current = buffer + current->nextListNode;
                //current = current + current->nextListNode;
            i = 0;
        }
    }
    return OK_SUCCESS;
}

ptrdiff_t getListHead(ptrdiff_t *index, uint32_t id) {

    if (index == NULL) return NULL;
    return index[id];
}

int destroyNodeIndex(ptrdiff_t *index) {

    if (index == NULL) return IND_EMPTY;
    free(index);
    return OK_SUCCESS;
}

