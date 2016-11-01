#include "buffer.h"


list_node *createBuffer(uint32_t buffer_size) {

    int pos = 0, n = 0;
    list_node *buffer = NULL;
    buffer = malloc(sizeof(list_node) * buffer_size);
    //if(tmp == NULL) return ALLOC_FAIL;

    for (pos = 0; pos < buffer_size; pos++) {
        for (n = 0; n < N; n++) {
            buffer[pos].neighbor[n] = DEFAULT; //sumvasi ta id
            buffer[pos].empty = 'y';
            buffer[pos].nextListNode = -1;
        }
    }
    return buffer;
}

ptrdiff_t allocNewNode(list_node **buffer, uint32_t *buffer_size) {

    int i = 0;
    ptrdiff_t offset = 0;
    uint32_t pos = 0;
    list_node *tmp = NULL;

    if (*buffer == NULL) return -1; //return error

    while (pos < (*buffer_size)) {
        if ((*buffer)[pos].empty == 'y') {
            tmp = &(*buffer)[pos];
            offset = tmp - (*buffer);
            //
            (*buffer)[pos].empty = 'n';
            (*buffer)->nextListNode = -1;
            for(i = 0; i < N; i++) (*buffer)[pos].neighbor[i] = DEFAULT;
            //
            return (offset);
        } //den paizei na doylevei
        pos++;
    }

    reallocBuffer(buffer, &(*buffer_size));
    return allocNewNode(buffer, &(*buffer_size));

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
