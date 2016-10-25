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

int checkIfExists(list_node *index[], uint32_t id){
    if(index[id] != NULL) return ALR_EXISTS;
    return NOT_EXIST;
}

int addEdge(list_node *index[], uint32_t id, uint32_t neighbor, list_node *buffer){

    int i = 0;
    list_node *current = NULL;


    while(i < N){
        if(current->neighbor[i] == neighbor) return ALR_CONNECTED;
        if(current->neighbor[i] == DEFAULT){
            current->neighbor[i] = neighbor;
            break;
        }
        i++;
        if(i == N){
            if(current->nextListNode == NULL) {
                current->nextListNode = allocNewNode(buffer); //elegxos gia NULL realloc
                if(current->nextListNode == NULL){
                    reallocBuffer(buffer);
                    current->nextListNode = allocNewNode(buffer);
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
int insertNode(list_node *index[], uint32_t id, uint32_t neighbor, list_node *buffer){

    if(index == NULL) return IND_EMPTY;

    index[id] = allocNewNode(buffer);//tuxaia seira sto buffer etsi

    if(index[id] == NULL) return ALLOC_FAIL;

    index[id]->empty = 'n';
    if(neighbor != DEFAULT){
        addEdge(index,id,neighbor,buffer);
        //free neighbors?
    }


    return OK_SUCCESS;
}
