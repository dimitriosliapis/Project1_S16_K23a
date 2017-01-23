#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#define THREAD_POOL_SIZE 4

pthread_mutex_t mtx;
pthread_mutex_t id_mtx;
pthread_cond_t cond_nonfinished;
pthread_cond_t cond_start;

//Buffer
typedef struct B_Node_t{
    char query[64];
    uint32_t line;
    struct B_Node_t *next;
} B_Node;

typedef struct Buffer_t {
    B_Node *first;
    B_Node *last;
} Buffer;

#endif
