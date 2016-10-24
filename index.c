#include "index.h"


NodeIndex createNodeIndex(){

    NodeIndex tmp = NULL;
    int i = 0;

    tmp = malloc(sizeof(list_node) * N);
    for(i = 0; i < index_size; i++){
        tmp[i] = NULL;
    }

    return tmp;
}

int insertNode(NodeIndex *index, uint32_t id, uint32_t *neighbors, Buffer *buffer){

    int i = 0;


    if(*index == NULL) return IND_EMPTY;

    if((*index[id]) != NULL) return ALR_EXISTS;

    *index[id] = allocNewNode(&buffer);//tuxaia seira sto buffer etsi

    if(*index[id] == NULL) return ALLOC_FAIL;

    if(neighbors != NULL){
        for(i = 0; i < N; i++){
            (*index[id])->neighbor[i] = neighbors[i];
        }
        //free neighbors?
    }


    return OK_SUCCESS;
}
