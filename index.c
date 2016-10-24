#include "index.h"


Index createNodeIndex(){

    Index tmp = NULL;
    int i = 0;

    tmp = malloc(sizeof(NodeIndex) * N);
    for(i = 0; i < index_size; i++){
        tmp[i].first = NULL;
    }

    return tmp;
}

int insertNode(Index *index, uint32_t id, uint32_t *neighbors, Buffer *buffer){

    int i = 0;

    if(*index == NULL) return IND_EMPTY;

    if((*index[id]).first != NULL) return ALR_EXISTS;

    (*index[id]).first = allocNewNode(&buffer);//tuxaia seira sto buffer etsi

    if((*index[id]).first == NULL) return ALLOC_FAIL;

    if(neighbors != NULL){
        for(i = 0; i < N; i++){
            (*index[id]).first->neighbor[i] = neighbors[i];
        }
    }

    //free neighbors?

    return OK_SUCCESS;
}
