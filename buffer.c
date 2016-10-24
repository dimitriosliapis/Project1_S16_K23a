#include "buffer.h"


Buffer createBuffer(){

    int pos = 0, n = 0;
    Buffer tmp = NULL;

    //if(tmp != NULL) return PTR_NOT_NULL;

    tmp = malloc(sizeof(list_node) * buffer_size);
    //if(tmp == NULL) return ALLOC_FAIL;


    for(pos = 0; pos < buffer_size; pos++){
        for(n = 0; n < N; n++){
            tmp[pos].neighbor[n] = 0; //sumvasi ta id > 0
            tmp[pos].nextListNode = NULL;
        }
    }
    return tmp;
}

int reallocBuffer(Buffer *b){

    int pos = 0, n = 0;
    void *tmp = NULL;

    buffer_size *= 2;

    tmp = realloc(*b,buffer_size * sizeof(list_node));
    if(tmp == NULL) return ERROR;

    for(pos = buffer_size/2; pos < buffer_size; pos++){
        for(n = 0; n < N; n++){
            (*b[pos]).neighbor[n] = 0;
        }
    }
    return OK_SUCCESS;
}

list_node* allocNewNode(Buffer *b){

    int err = 0;
    uint32_t pos = 0;
    //if(*b == NULL) *b = malloc(sizeof(list_node) * buffer_size); //return error
    if(*b == NULL) return NULL; //return error

    while(pos < buffer_size){
     //   if((*b[pos]).neighbor[0] == 0) return (*b + pos*sizeof(list_node)); //den paizei na doylevei
        if((*b[pos]).neighbor[0] == 0) return b[pos]; //den paizei na doylevei
        pos++;
    }

    return NULL;

}

int destroyBuffer(Buffer *b){

   free(*b);
    return OK_SUCCESS;
}

