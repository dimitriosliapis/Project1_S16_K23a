#include "thread.h"
#include <stdio.h>
#include <string.h>

void place_to_buffer(char *query, Buffer_t *buffer) {
    pthread_mutex_lock(&mutex);
    while(buffer->count == QUERY_SIZE) {
        printf("Buffer full.\n");
        pthread_cond_wait(&cond_nonfull, &mutex);
    }
    buffer->end = (buffer->end + 1)%QUERY_SIZE;
    strcpy(&buffer->querry[buffer->end], query);
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