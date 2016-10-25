#include "index.h"


unsigned long* createNodeIndex(){

    unsigned long* tmp = NULL;
    int i = 0;

    tmp = malloc(sizeof(unsigned long) * index_size);
    for(i = 0; i < index_size; i++){
        tmp[i] = 0;
    }

    return tmp;
}

int reallocNodeIndex(unsigned long *index, int id) {

    uint32_t realloc_size = index_size;
    uint32_t a = 0;

    while(id >= realloc_size) realloc_size = realloc_size*2;	//Double size until id fits
    index = realloc(index, realloc_size*sizeof(unsigned long));

    for(a = index_size ; a < realloc_size ; a++) {	//Initialize new index nodes
        //Connect id index node to buffer -> tha ginei meta stin insertNode
        index[a] = 0;
    }
    index_size = realloc_size;
    return OK_SUCCESS;
}

unsigned long getListHead(unsigned long *index, uint32_t id){

    if(index == NULL) return NULL;
    return index[id];
}

int checkIfExists(unsigned long *index, uint32_t id){
    if(index[id] != 0) return ALR_EXISTS;
    return NOT_EXIST;
}

int addEdge(unsigned long *index, uint32_t id, uint32_t neighbor, list_node *buffer){

    int i = 0;
    list_node *current = NULL;
    unsigned long offset = 0;

    offset = getListHead(index,id);
    current = buffer + offset;

    while(i < N){
        if(current->neighbor[i] == neighbor) return ALR_CONNECTED;
        if(current->neighbor[i] == DEFAULT){
            current->neighbor[i] = neighbor;
            break;
        }
        i++;
        if(i == N){
            if(current->nextListNode == NULL) {
                offset = allocNewNode(buffer); //elegxos gia NULL realloc
                current->nextListNode = buffer + offset; //elegxos gia NULL realloc
                if(current->nextListNode == NULL){
                    reallocBuffer(buffer);
                    offset = allocNewNode(buffer);
                    current->nextListNode = buffer + offset;
                    //current->nextListNode = &buffer[buffer_size/2];
                }
            }
            else
                current = current->nextListNode;
            i = 0;
        }
    }
    return OK_SUCCESS;
}
int insertNode(unsigned long *index, uint32_t id, uint32_t neighbor, list_node *buffer){

    unsigned long offset = 0;
    list_node *new = NULL;

    if(index == NULL) return IND_EMPTY;

    offset = allocNewNode(buffer);
    index[id] = offset;//tuxaia seira sto buffer etsi

    if(index[id] == NULL) return ALLOC_FAIL;

    new = buffer + index[id];

    new->empty = 'n';
    if(neighbor != DEFAULT){
        addEdge(index,id,neighbor,buffer);
        //free neighbors?
    }


    return OK_SUCCESS;
}

int destroyNodeIndex(unsigned long *index){

    free(index);
    return OK_SUCCESS;
}
