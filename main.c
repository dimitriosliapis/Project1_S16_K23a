#include <sys/time.h>
#include "grail.h"

#define RES_INIT 170000000

/****************GLOBALS***********************/

list_node *buffer_in = NULL, *buffer_out = NULL;
ind *index_in = NULL, *index_out = NULL;
uint32_t buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE, index_size_in = IND_SIZE, index_size_out = IND_SIZE;
ptrdiff_t available_in = 0, available_out = 0;
SCC *scc = NULL;
Buffer *buffer = NULL;
GrailIndex *grail = NULL;

CC *cc = NULL;
uint32_t cc_size = 0;

int start = 0, finished = 0, id = 1, end = 0, res_size, *results;

struct timeval tv1, tv2;



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

/**************************THREADS************************************/

void place_to_buffer(char *query, Buffer *buffer, uint32_t line) {

    B_Node *new = NULL, *tmp = NULL;

    if (query == NULL || buffer == NULL) return;

    new = malloc(sizeof(B_Node));
    strcpy(new->query, query);
    new->next = NULL;
    new->line = line;

    // printf("INSERTING QUERY: %s\n", query);

    if (buffer->first == NULL) {
        buffer->first = new;
        buffer->last = new;
        return;
    }

    tmp = buffer->last;
    if (tmp != NULL)
        tmp->next = new;
    buffer->last = new;

}

B_Node *remove_from_buffer(Buffer *buffer) {

    B_Node *node = NULL, *tmp = NULL;

    if (buffer->first != NULL) {

        tmp = buffer->first;

        node = malloc(sizeof(B_Node));
        node->line = tmp->line;
        strcpy(node->query, tmp->query);
        node->next = NULL;

        buffer->first = tmp->next;
        free(tmp);

        //printf("REMOVING QUERY: %s\n", node->query);

        return node;

    } else return NULL;
}

/*
 * static_graph_worker
*/

void *worker(void *ptr) {

    uint32_t N1, N2, scc_source;
    int line, thread_id, steps;
    uint32_t local_version = 0;
    Queue *frontierF = NULL, *frontierB = NULL;
    B_Node *job = NULL;

    pthread_mutex_lock(&id_mtx);
    thread_id = id;
    id++;
    pthread_mutex_unlock(&id_mtx);

    frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    frontierB = createQueue();  // synoro tou bfs apo ton stoxo pros thn arxh

    while (1) {

        pthread_mutex_lock(&mtx);

        while (start == 0)
            pthread_cond_wait(&cond_start, &mtx);

        job = remove_from_buffer(buffer);

        if (job == NULL) {

            pthread_cond_signal(&cond_nonfinished);
            pthread_mutex_unlock(&mtx);
            if (end) break;
            continue;
        } else {

            line = job->line;
            pthread_mutex_unlock(&mtx);
        }

        toID(job->query, &N1, &N2);

        if (lookup(index_out, N1, index_size_out) == ALR_EXISTS && lookup(index_in, N2, index_size_in) == ALR_EXISTS) {

            scc_source = scc->id_belongs_to_component[N1];

            if (isReachableGrailIndex(grail, N1, N2, scc) == YES) { // YES
                local_version++;
                steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB,
                             local_version, thread_id, scc_source);
                results[line] = steps;

            } else if (isReachableGrailIndex(grail, N1, N2, scc) == MAYBE) {    // MAYBE
                local_version++;
                steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB,
                             local_version, thread_id, DEFAULT);
                results[line] = steps;
            } else  // NO
                results[line] = -1;

        } else
            results[line] = -1;

        free(job);
        pthread_mutex_lock(&mtx);
        finished++;
        pthread_mutex_unlock(&mtx);
    }

    empty(frontierB);
    empty(frontierF);

    pthread_exit(0);
}

/*
 * dynamic_graph_worker
*/

void *worker_dynamic(void *ptr) {

    uint32_t N1, N2;
    uint32_t line;
    int thread_id, steps;
    Queue *frontierF = NULL, *frontierB = NULL;
    B_Node *job = NULL;

    pthread_mutex_lock(&id_mtx);
    thread_id = id;
    id++;
    pthread_mutex_unlock(&id_mtx);

    frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    frontierB = createQueue();  // synoro tou bfs apo ton stoxo pros thn arxh

    while (1) {

        pthread_mutex_lock(&mtx);
        while (start == 0)
            pthread_cond_wait(&cond_start, &mtx);

        job = remove_from_buffer(buffer);

        if (job == NULL) {

            pthread_cond_signal(&cond_nonfinished);
            pthread_mutex_unlock(&mtx);
            if (end) break;
            continue;
        } else {

            line = job->line;
            pthread_mutex_unlock(&mtx);
        }

        toID(job->query, &N1, &N2);

        if (lookup(index_out, N1, index_size_out) == ALR_EXISTS && lookup(index_in, N2, index_size_in) == ALR_EXISTS) {

            pthread_mutex_lock(&cc_mtx);

            if (cc->cc_index[N1].id == cc->cc_index[N2].id) {

                pthread_mutex_unlock(&cc_mtx);

                steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB,
                             line, thread_id, -1);

                results[line] = steps;
            } else {

                if (searchUpdateIndex(*cc, N1, N2, line, thread_id) == FOUND) {

                    pthread_mutex_unlock(&cc_mtx);

                    steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB,
                                 line, thread_id, -1);

                    results[line] = steps;

                    pthread_mutex_lock(&cc_mtx);

                    cc->metricVal--;

                    if (cc->metricVal == 0) {

                        if (index_size_in > index_size_out) cc_size = index_size_in;
                        else cc_size = index_size_out;
                        cc->cc_max = updateCCIndex(cc, line, cc_size, thread_id);
                        cc->metricVal = METRIC;

                    }

                    pthread_mutex_unlock(&cc_mtx);
                } else {
                    pthread_mutex_unlock(&cc_mtx);
                    results[line] = -1;
                }
            }
        } else results[line] = -1;

        free(job);
        pthread_mutex_lock(&mtx);
        finished++;
        pthread_mutex_unlock(&mtx);
    }

    empty(frontierB);
    empty(frontierF);

    pthread_exit(0);
}

/*********************MAIN**********************/

int main(int argc, char *argv[]) {

    FILE *Graph = NULL, *Queries = NULL;
    uint32_t i = 0, print_start = 0, print = 1;
    uint32_t N1, N2, scc_size = 0;
    uint32_t version = 0, line = 0;
    pthread_t *workers_t;
    ptrdiff_t ret = 0;
    char str[64], which[64];
    int stat = 0;

    // orismata
    if (argc == 3) {
        Graph = fopen(argv[1], "r");
        Queries = fopen(argv[2], "r");
    } else {
        printf("Datasets missing");
        return 0;
    }
    fgets(which, sizeof(which), Queries);
    if (strncmp(which, "STATIC", 6) == 0) stat = 1;

    // zeugh indexes kai buffers
    buffer_in = createBuffer(buffer_size_in);
    index_in = createNodeIndex(index_size_in, 0);
    buffer_out = createBuffer(buffer_size_out);
    index_out = createNodeIndex(index_size_out, stat);

    // results array
    res_size = RES_INIT;
    results = malloc(RES_INIT * sizeof(int));
    for (i = 0; i < res_size; i++)
        results[i] = -2;

    // job buffer
    buffer = malloc(sizeof(Buffer));
    buffer->first = NULL;
    buffer->last = NULL;



        // mutexes and condition variables initialization
    pthread_mutex_init(&mtx, 0);
    pthread_mutex_init(&id_mtx, 0);
    pthread_mutex_init(&cc_mtx, 0);
    pthread_cond_init(&cond_nonfinished, 0);
    pthread_cond_init(&cond_start, 0);

    // time
    gettimeofday(&tv1, NULL);

    fgets(str, sizeof(str), Graph);
    while (str[0] != 'S') {

        toID(str, &N1, &N2);

        if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
            insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out, &available_out, stat);

        if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
            insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, &available_in, 0);

        ret = addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, &available_out, 1, 0);

        if(ret != ALR_CONNECTED)
            addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, &available_in, 0, 0);

        fgets(str, sizeof(str), Graph);
    }

    fclose(Graph);

    // free unnecessary stuff
    /*for (i = 0; i < index_size_in; i++) {
        if (lookup(index_in, i, index_size_in) == ALR_EXISTS) {
            if (index_in[i].neighbors != NULL)
                delete(index_in[i].neighbors, HT_SMALL);
            index_in[i].neighbors = NULL;
        }
    }*/
    for (i = 0; i < index_size_out; i++) {
        if (lookup(index_out, i, index_size_out) == ALR_EXISTS) {
            if (index_out[i].neighbors != NULL)
                delete(index_out[i].neighbors, HT_SMALL);
            index_out[i].neighbors = NULL;
        }
    }

    gettimeofday(&tv2, NULL);
    printf("%f sec: Graph creation\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

    //fgets(str, sizeof(str), Queries);
    strcpy(str, which);
    // static graph creation
    if (strncmp(str, "STATIC", 6) == 0) {

        // worker thread pool
        workers_t = malloc(THREAD_POOL_SIZE * sizeof(pthread_t));
        for (i = 0; i < THREAD_POOL_SIZE; i++)
            pthread_create(&workers_t[i], 0, worker, 0);

        if (index_size_in > index_size_out)
            scc_size = index_size_in;
        else
            scc_size = index_size_out;

        // scc estimation
        version++;
        //scc = estimateStronglyConnectedComponents(index_out, buffer_out, scc_size, version);
        scc = estimateStronglyConnectedComponents_iterative(index_out, buffer_out, index_size_out, scc_size);

        gettimeofday(&tv2, NULL);
        printf("%f sec: SCC estimation\n",
               (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double) (tv2.tv_sec - tv1.tv_sec));

        // grail index creation
        version++;
        grail = buildGrailIndex(index_out, buffer_out, scc, version);

        // free unnecessary stuff
        for (i = 0; i < scc->components_count; i++)
            free(scc->components[i].included_node_ids);
        free(scc->components);

        fgets(str, sizeof(str), Queries);

        while (!feof(Queries)) {

            pthread_mutex_lock(&mtx);

            while (finished < line)
                pthread_cond_wait(&cond_nonfinished, &mtx);

            for (i = print_start; i < line; i++)
                printf("%d\n", results[i]);
            print_start = line;

            while (str[0] != 'F') {

                place_to_buffer(str, buffer, line);
                line++;
                fgets(str, sizeof(str), Queries);

            }

            //finished = 0;
            start = 1;
            pthread_mutex_unlock(&mtx);
            pthread_cond_broadcast(&cond_start);

            fgets(str, sizeof(str), Queries);
        }

        end = 1;
        res_size = line;
        fclose(Queries);
    } else {

        //to megethos tou cc tha einai osoi einai oi komvoi sinolika diladi
        //to max twn komvwn tou index_in kai index_out

        if (index_size_in > index_size_out) cc_size = index_size_in;
        else cc_size = index_size_out;

        version++;
        cc = createCCIndex(cc_size, index_in, index_out, buffer_in, buffer_out, index_size_in, index_size_out, version);

        cc->u_size = cc->cc_max;
        cc->metricVal = METRIC;

        initUpdateIndex(cc);
        gettimeofday(&tv2, NULL);
        printf("%f sec: CC create\n",
               (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double) (tv2.tv_sec - tv1.tv_sec));
        // worker thread pool
        workers_t = malloc(THREAD_POOL_SIZE * sizeof(pthread_t));

        for (i = 0; i < THREAD_POOL_SIZE; i++)
            pthread_create(&workers_t[i], 0, worker_dynamic, 0);

        while (!feof(Queries)) {

            pthread_mutex_lock(&mtx);

            while (finished < line)
                pthread_cond_wait(&cond_nonfinished, &mtx);

            for (i = print_start; i < line; i++)
                printf("%d\n", results[i]);
            print_start = line;

            while (str[0] != 'F') {

                if (str[0] == 'A') {

                    toID(str, &N1, &N2);

                    if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
                        insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out, &available_out, stat);

                    if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
                        insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, &available_in, 0);

                    ret = addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, &available_out, 1, line);

                    if(ret != ALR_CONNECTED)
                        addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, &available_in, 0, line);

                    refreshUpdateIndex(cc, N1, N2);
                } else {
                    place_to_buffer(str, buffer, line);
                    line++;
                }

                fgets(str, sizeof(str), Queries);
            }

            //finished = 0;
            start = 1;
            pthread_mutex_unlock(&mtx);
            pthread_cond_broadcast(&cond_start);

            fgets(str, sizeof(str), Queries);
        }

        end = 1;
        res_size = line;
        fclose(Queries);
    }

    pthread_mutex_lock(&mtx);

    while (finished < line)
        pthread_cond_wait(&cond_nonfinished, &mtx);

    pthread_mutex_unlock(&mtx);

    for (i = print_start; i < line; i++)
        printf("%d\n", results[i]);

    for (i = 0; i < THREAD_POOL_SIZE; i++)
        pthread_join(workers_t[i], 0);

    gettimeofday(&tv2, NULL);
    printf("%f sec: The End\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

    destroyBuffer(buffer_in);
    destroyBuffer(buffer_out);
    destroyNodeIndex(index_in, index_size_in, -1);
    destroyNodeIndex(index_out, index_size_out, stat);
    free(results);
    free(buffer);
    free(workers_t);

    if (scc != NULL)
        destroyStronglyConnectedComponents(scc);
    if (grail != NULL)
        destroyGrailIndex(grail);
    if (cc != NULL)
        destroyCCIndex(cc);

    pthread_cond_destroy(&cond_nonfinished);
    pthread_cond_destroy(&cond_start);
    pthread_mutex_destroy(&id_mtx);
    pthread_mutex_destroy(&mtx);

    return 0;
}