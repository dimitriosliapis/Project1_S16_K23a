#include "buffer.h"


list_node* createBuffer(){

    int pos = 0, n = 0;
    list_node* tmp = NULL;

    //if(tmp != NULL) return PTR_NOT_NULL;

    tmp = malloc(sizeof(list_node) * buffer_size);
    //if(tmp == NULL) return ALLOC_FAIL;


    for(pos = 0; pos < buffer_size; pos++){
        for(n = 0; n < N; n++){
            tmp[pos].neighbor[n] = 0; //sumvasi ta id
            tmp[pos].empty = 'y';
            tmp[pos].nextListNode = NULL;
        }
    }
    return tmp;
}

int reallocBuffer(list_node *buffer){

    int pos = 0, n = 0;
    void *tmp = NULL;

    buffer_size *= 2;

    tmp = realloc(buffer,buffer_size * sizeof(list_node));
    if(tmp == NULL) return ERROR;

    for(pos = buffer_size/2; pos < buffer_size; pos++) {
        for(n = 0; n < N; n++) {
            buffer[pos].neighbor[n] = DEFAULT;
        }
        buffer[pos].empty = 'y';
        buffer[pos].nextListNode = NULL;
    }
    return OK_SUCCESS;
}


list_node* allocNewNode(list_node *buffer){

    int err = 0;
    uint32_t pos = 0;


    if(buffer == NULL) return NULL; //return error

    while(pos < buffer_size){
        if(buffer[pos].empty == 'y') return &(buffer[pos]); //den paizei na doylevei
        pos++;
    }

    return NULL;

}

int destroyBuffer(list_node *buffer){

   free(buffer);
    return OK_SUCCESS;
}

