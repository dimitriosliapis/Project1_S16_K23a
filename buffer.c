#include "buffer.h"


list_node *createBuffer(uint32_t buffer_size) {

    int pos = 0, n = 0;
    list_node *buffer = NULL;
    buffer = malloc(sizeof(list_node) * buffer_size);

    for (pos = 0; pos < buffer_size; pos++) {
        for (n = 0; n < N; n++) {
            buffer[pos].neighbor[n] = DEFAULT;  //sumvasi gia ta id
        }
        buffer[pos].empty = 'y';            //adeios
        buffer[pos].nextListNode = -1;      //den exei epomeno
    }
    return buffer;
}

ptrdiff_t allocNewNode(list_node **buffer, uint32_t *buffer_size, ptrdiff_t start) {

    int i = 0;
    ptrdiff_t pos = start;                      //exei ena starting point gia na min psaxnei ap to 0

    if (*buffer == NULL){
        return -1; //return error
    }

    while (pos < (*buffer_size)) {              //psaxnei ton prwto keno komvo kai ton epistrefei
        if ((*buffer)[pos].empty == 'y') {
            (*buffer)[pos].empty = 'n';
            return (pos);
        }
        pos++;
    }
    //an den exei allo keno kanei realloc
    reallocBuffer(&(*buffer), &(*buffer_size));
    return allocNewNode(&(*buffer), &(*buffer_size), pos);   //ksanakalei ton eauto tis me to kainourgio megethos

}

int reallocBuffer(list_node **buffer, uint32_t *buffer_size) {

    int pos = 0, n = 0;
    void *new = NULL;

    *buffer_size *= 2;

    new = realloc(*buffer, *buffer_size * sizeof(list_node));
    if (new == NULL) return ERROR;
    *buffer = new;

    for (pos = *buffer_size / 2; pos < *buffer_size; pos++) {
        for (n = 0; n < N; n++) {
            (*buffer)[pos].neighbor[n] = DEFAULT;
        }
        (*buffer)[pos].empty = 'y';
        (*buffer)[pos].nextListNode = -1;
    }
    return OK_SUCCESS;
}

int destroyBuffer(list_node *buffer) {

    free(buffer);
    return OK_SUCCESS;
}
