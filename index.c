#include "index.h"


ptrdiff_t* createNodeIndex(){

    ptrdiff_t* tmp = NULL;
    int i = 0;

    tmp = malloc(sizeof(ptrdiff_t) * index_size);
    for(i = 0; i < index_size; i++){
        tmp[i] = -1;
    }

    return tmp;
}

int reallocNodeIndex(ptrdiff_t *index, int id) {

    uint32_t realloc_size = index_size;
    uint32_t a = 0;

    while(id >= realloc_size) realloc_size = realloc_size*2;	//Double size until id fits
    index = realloc(index, realloc_size*sizeof(ptrdiff_t));

    for(a = index_size ; a < realloc_size ; a++) {	//Initialize new index nodes
        //Connect id index node to buffer -> tha ginei meta stin insertNode
        index[a] = -1;
    }
    index_size = realloc_size;
    return OK_SUCCESS;
}

ptrdiff_t getListHead(ptrdiff_t *index, uint32_t id){

    if(index == NULL) return NULL;
    return index[id];
}

int checkIfExists(ptrdiff_t *index, uint32_t id){
    if(index[id] != -1) return ALR_EXISTS;
    return NOT_EXIST;
}

int addEdge(ptrdiff_t *index, uint32_t id, uint32_t neighbor, list_node *buffer){

    int i = 0;
    ptrdiff_t offset = 0;

    offset = getListHead(index,id);
    list_node *current = buffer + offset;

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
int insertNode(ptrdiff_t *index, uint32_t id, uint32_t neighbor, list_node *buffer){

    ptrdiff_t offset = 0;
    list_node *new = NULL;

    if(index == NULL) return IND_EMPTY;

    offset = allocNewNode(buffer);
    index[id] = offset;//tuxaia seira sto buffer etsi

    //if(index[id] == NULL) return ALLOC_FAIL;     prepei na mpei elegxos

    new = buffer + index[id];

    new->empty = 'n';
    if(neighbor != DEFAULT){
        addEdge(index,id,neighbor,buffer);
        //free neighbors?
    }


    return OK_SUCCESS;
}

int destroyNodeIndex(ptrdiff_t *index){

    free(index);
    return OK_SUCCESS;
}
