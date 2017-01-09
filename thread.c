#include "thread.h"
#include <string.h>

int toID(char *str, uint32_t *N1, uint32_t *N2) {
    uint32_t i, j, id;
    if (str == NULL)
        return 0;
    if ((str[0] < '0') || (str[0] > '9'))
        i = 2;
    else
        i = 0;
    for (j = 0; j < 2; j++) {
        id = 0;
        while ((str[i] >= '0') && (str[i] <= '9')) {
            id = 10 * id + (str[i] - '0');
            i++;
        }
        if (j == 0)
            *N1 = id;
        else
            *N2 = id;
        i++;
    }
    return 1;
}

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

char* remove_from_buffer(Buffer_t *buffer) {
    char *query;

    query = malloc(64*sizeof(char));
    memset(query, '\0', 64);

    pthread_mutex_lock(&mutex);
    while(buffer->count == 0) {
        printf("Buffer empty.\n");
        pthread_cond_wait(&cond_nonempty, &mutex);
    }
    strcpy(query, &buffer->querry[buffer->start]);
    buffer->start = (buffer->start + 1)%QUERY_SIZE;
    buffer->count--;
    pthread_mutex_unlock(&mutex);
    return query;
}

void *master_thread_function(void *ptr) {
    arg *local = ptr;
    char str[64];
    fgets(str,sizeof(str), local->file);
    while (!feof(local->file) || str[0] == 'F') {
        place_to_buffer(*str, local->buffer);
    }
}

void *worker_thread_function(void *ptr){

    arg *local = ptr;
    char *query;
    uint32_t N1, N2;




    query = remove_from_buffer(local->buffer);

    toID(query, &N1, &N2);

    if (lookup(local->data->index_out, N1, local->data->index_size_out) == ALR_EXISTS && lookup(local->data->index_in, N2, local->data->index_size_in) == ALR_EXISTS && isReachableGrailIndex(local->data->grail,N1,N2,local->data->scc) == MAYBE) {
        local->data->version++;
        local->data->steps = bBFS(local->data->index_in, local->data->index_out, local->data->buffer_in, local->data->buffer_out, N1, N2, local->data->frontierF, local->data->frontierB, local->data->version);

        printf("%d\n", local->data->steps);//den tupwnei ta vazei ston pinaka me ta apotelesmata
    } else
        printf("-1\n");//den tupwnei ta vazei ston pinaka me ta apotelesmata

    local->data->version++;

}