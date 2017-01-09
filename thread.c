#include "thread.h"
#include <string.h>

void place_to_buffer(char query, Buffer_t *buffer) {
    pthread_mutex_lock(&mutex);
    while(buffer->count == QUERY_SIZE) {
        printf("Buffer full.\n");
        pthread_cond_wait(&cond_nonfull, &mutex);
    }
    buffer->end = (buffer->end + 1)%QUERY_SIZE;
    strcpy(&buffer->querry[buffer->end], &query);
    buffer->count++;
    pthread_mutex_unlock(&mutex);
}

char remove_from_buffer(Buffer_t *buffer) {
    char query[64];
    pthread_mutex_lock(&mutex);
    while(buffer->count == 0) {
        printf("Buffer empty.\n");
        pthread_cond_wait(&cond_nonempty, &mutex);
    }
    strcpy(query, &buffer->querry[buffer->start]);
    buffer->start = (buffer->start + 1)%QUERY_SIZE;
    buffer->count--;
    pthread_mutex_unlock(&mutex);
    return *query;
}

void *master_thread_function(void *ptr) {
    arg *local = ptr;
    char str[64];
    fgets(str,sizeof(str), local->file);
    while (!feof(local->file) || str[0] == 'F') {
        place_to_buffer(*str, local->buffer);
    }
}