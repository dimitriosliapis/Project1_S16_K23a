#include "buffer.h"


list_node *createBuffer(uint32_t buffer_size) {

    int pos = 0, n = 0;
    list_node *buffer = NULL;
    buffer = malloc(sizeof(list_node) * buffer_size);

    for (pos = 0; pos < buffer_size; pos++) {
        for (n = 0; n < N; n++) {
            buffer[pos].neighbor[n] = DEFAULT;  //sumvasi gia ta id
        }
        buffer[pos].nextListNode = -1;      //den exei epomeno
    }
    return buffer;
}

ptrdiff_t allocNewNode(list_node **buffer, uint32_t *buffer_size, ptrdiff_t available) {

    if (*buffer == NULL) return -1; //return error
    if (available >= (*buffer_size)) {//tsekarei an o epomenos komvos xwraei sto buffer
        //printf("gia buffer %d buffer_size = %d\n", &(*buffer), *buffer_size);
        reallocBuffer(&(*buffer), &(*buffer_size));
        //printf("gia buffer %d buffer_size = %d\n", &(*buffer), *buffer_size);
    }
    return (available);

}

int reallocBuffer(list_node **buffer, uint32_t *buffer_size) {

    int pos = 0, n = 0;
    void *new = NULL;

    (*buffer_size) *= 2;

    new = realloc(*buffer, (*buffer_size) * sizeof(list_node));
    if (new == NULL) return ERROR;
    *buffer = new;

    for (pos = (*buffer_size) / 2; pos < *buffer_size; pos++) {
        for (n = 0; n < N; n++) {
            (*buffer)[pos].neighbor[n] = DEFAULT;
        }
        (*buffer)[pos].nextListNode = -1;
    }
    return OK_SUCCESS;
}

int destroyBuffer(list_node *buffer) {

    free(buffer);
    return OK_SUCCESS;
}
