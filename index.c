#include "index.h"


list_node** createNodeIndex(){

    list_node** tmp = NULL;
    int i = 0;

    tmp = malloc(sizeof(list_node*) * index_size);
    for(i = 0; i < index_size; i++){
        tmp[i] = NULL;
    }

    return tmp;
}

int reallocNodeIndex(list_node **index, int id) {

    int realloc_size = index_size;
    int a = 0;

    while(id >= realloc_size) realloc_size = realloc_size*2;	//Double size until id fits
    index = realloc(index, realloc_size*sizeof(list_node*));

    for(a = index_size ; a < realloc_size ; a++) {	//Initialize new index nodes
        //Connect id index node to buffer -> tha ginei meta stin insertNode
        index[a] = NULL;
    }
    return OK_SUCCESS;
}

int insertNode(list_node *index[], uint32_t id, uint32_t *neighbors, list_node *buffer){

    int i = 0;


    if(index == NULL) return IND_EMPTY;

    if(index[id] != NULL) return ALR_EXISTS;

    index[id] = allocNewNode(buffer);//tuxaia seira sto buffer etsi

    if(index[id] == NULL) return ALLOC_FAIL;

    index[id]->empty = 'n';
    if(neighbors != NULL){
        for(i = 0; i < N; i++){
            index[id]->neighbor[i] = neighbors[i];
        }
        //free neighbors?
    }


    return OK_SUCCESS;
}
