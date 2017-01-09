#include <pthread.h>

#define QUERY_SIZE 5

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

void place_to_buffer(char *query, Buffer_t *buffer);
char remove_from_buffer(Buffer_t *buffer);
