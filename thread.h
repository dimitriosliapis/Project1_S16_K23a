#include <pthread.h>
#include <stdio.h>
#include "grail.h"

#define QUERY_SIZE 5
#define THREAD_POOL_SIZE 2

pthread_mutex_t mutex;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

struct JobScheduler{
    int execution_threads; // number of execution threads
    jobQueue* q; // a queue that holds submitted jobs / tasks
    p_thread_t* tids; // execution threads
// mutex, condition variable, ...
};

//Buffer
typedef struct Buffer_t {
    int start;
    int end;
    int count;
    char *querry;
} Buffer_t;

typedef struct work_data_t{

    list_node *buffer_in, *buffer_out;
    ind *index_in, *index_out;
    SCC *scc;
    GrailIndex *grail;

    uint32_t buffer_size_in, buffer_size_out, index_size_in, index_size_out;
    uint32_t scc_size;

    ptrdiff_t available_in, available_out;
    Queue *frontierF, *frontierB;
    ht_Node *explored;
    uint32_t version;
    int steps;
}work_data;

typedef struct arg_t {
    FILE *file;
    Buffer_t *buffer;
    work_data *data;
} arg;

int toID(char *, uint32_t *, uint32_t *);

void place_to_buffer(char query, Buffer_t *buffer);
char* remove_from_buffer(Buffer_t *buffer);

void *master_thread_function(void *ptr);
void *worker_thread_function(void *ptr);