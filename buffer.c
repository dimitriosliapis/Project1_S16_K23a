#include "buffer.h"


list_node *createBuffer() {

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

ptrdiff_t allocNewNode(list_node *buffer) {

    int err = 0;
    ptrdiff_t offset = 0;
    uint32_t pos = 0;
    list_node *tmp;

    if (buffer == NULL) return -1; //return error

    while (pos < buffer_size) {
        if (buffer[pos].empty == 'y') {
            tmp = &buffer[pos];
            offset = tmp - buffer;
            return (offset);
        } //den paizei na doylevei
        pos++;
    }

    buffer = reallocBuffer(buffer);
    allocNewNode(buffer);

}

int reallocBuffer(list_node *buffer) {

    int pos = 0, n = 0;
    void *tmp = NULL;

    buffer_size *= 2;

    tmp = realloc(buffer, buffer_size * sizeof(list_node));
    if (tmp == NULL) return ERROR;

    for (pos = buffer_size / 2; pos < buffer_size; pos++) {
        for (n = 0; n < N; n++) {
            buffer[pos].neighbor[n] = DEFAULT;
        }
        buffer[pos].empty = 'y';
        buffer[pos].nextListNode = -1;
    }
    return OK_SUCCESS;
}

int destroyBuffer(list_node *buffer) {

    free(buffer);
    return OK_SUCCESS;
}
