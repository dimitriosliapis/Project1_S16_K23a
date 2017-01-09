#include <pthread.h>



struct JobScheduler{
    int execution_threads; // number of execution threads
    jobQueue* q; // a queue that holds submitted jobs / tasks
    p_thread_t* tids; // execution threads
// mutex, condition variable, ...
};