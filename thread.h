#ifndef PROJECT1_S16_K23A_THREAD_H
#define PROJECT1_S16_K23A_THREAD_H


#include <pthread.h>
#include <stdio.h>
#include "globals.h"

#define QUERY_SIZE 8
#define EMPTY -2
#define RES_INIT 70000
#define START -1
#define FINISHED 1
#define CONTINUE 0
#define WAIT 2

pthread_mutex_t mutex;
pthread_mutex_t mutexb;
pthread_mutex_t vmutex;
pthread_cond_t cond_start;
pthread_cond_t cond_next;

int status;
int finished;
int max_id;
int max;


//Buffer
typedef struct B_Node_t{
    char query[64];
    int line;
    struct B_Node_t *next;
}B_Node;

typedef struct Buffer_t {
    B_Node *first;
    B_Node *last;
} Buffer;

typedef struct work_data_t{

    list_node *buffer_in, *buffer_out;
    ind *index_in, *index_out;
    SCC *scc;
    GrailIndex *grail;

    int buffer_size_in, buffer_size_out, index_size_in, index_size_out;
    int scc_size;

    ptrdiff_t available_in, available_out;
    Queue *frontierF, *frontierB;
    ht_Node *explored;
    int version;
    int steps;
    int current_job;
}work_data;

typedef struct arg_t {
    FILE *file;
    Buffer *buffer;
   // work_data *data;
    int *results;
    int res_size;
} arg;

int toID(char *, int *, int *);

void place_to_buffer(char *query, Buffer *buffer, int line);
int remove_from_buffer(Buffer *buffer, int *line, char **query);

void *master_thread_function(void *ptr);
void *worker_thread_function(void *ptr);
void *master_thread_function_dynamic(void *ptr);
void *worker_thread_function_dynamic(void *ptr);

#endif //PROJECT1_S16_K23A_THREAD_H
