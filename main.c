#include <sys/time.h>
#include "thread.h"


arg *thread_args;


int main(int argc, char *argv[]) {

    FILE *Graph = NULL;//, *Queries = NULL;
    //list_node *buffer_in = NULL, *buffer_out = NULL;
    //ind *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2;//, buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE, index_size_in = IND_SIZE, index_size_out = IND_SIZE;
    //ptrdiff_t available_in = 0, available_out = 0;
    //Queue *frontierF = NULL, *frontierB = NULL;
    //ht_Node *explored = NULL;
    //uint32_t version = 0;
    //int steps = 0;

//thread
   /* CC *cc = NULL;
    uint32_t cc_size = 0;
    uint32_t scc_size = 0;

    SCC *scc = NULL;
    GrailIndex *grail = NULL;*/

    int i = 0;

    thread_args = malloc(sizeof(arg));
    thread_args->data = malloc(sizeof(work_data));

    thread_args->data->buffer_size_in = BUFF_SIZE;
    thread_args->data->buffer_size_out = BUFF_SIZE;
    thread_args->data->index_size_in = IND_SIZE;
    thread_args->data->index_size_out = IND_SIZE;




    // orismata
    if (argc == 3) {
        Graph = fopen(argv[1], "r");
        thread_args->file = fopen(argv[2], "r");
    } else {
        printf("Datasets missing");
        return 0;
    }

    // zeugh indexes kai buffers
    thread_args->data->buffer_in = createBuffer(thread_args->data->buffer_size_in);
    thread_args->data->index_in = createNodeIndex(thread_args->data->index_size_in);
    thread_args->data->buffer_out = createBuffer(thread_args->data->buffer_size_out);
    thread_args->data->index_out = createNodeIndex(thread_args->data->index_size_out);

    thread_args->data->explored = createHashtable(HT_BIG);


    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    char str[64];
    fgets(str, sizeof(str), Graph);



    while (str[0] != 'S') {

        toID(str, &N1, &N2);

        if (lookup(thread_args->data->index_out, N1, thread_args->data->index_size_out) == NOT_EXIST)
            insertNode(&thread_args->data->index_out, N1, &thread_args->data->buffer_out, &thread_args->data->index_size_out, &thread_args->data->buffer_size_out, &thread_args->data->available_out);

        if (lookup(thread_args->data->index_in, N2, thread_args->data->index_size_in) == NOT_EXIST)
            insertNode(&thread_args->data->index_in, N2, &thread_args->data->buffer_in, &thread_args->data->index_size_in, &thread_args->data->buffer_size_in, &thread_args->data->available_in);

        addEdge(&thread_args->data->index_out, N1, N2, &thread_args->data->buffer_out, &thread_args->data->buffer_size_out, &thread_args->data->available_out);

        addEdge(&thread_args->data->index_in, N2, N1, &thread_args->data->buffer_in, &thread_args->data->buffer_size_in, &thread_args->data->available_in);

        fgets(str, sizeof(str), Graph);
    }

    fclose(Graph);

    thread_args->data->version = 0;

    if(thread_args->data->index_size_in > thread_args->data->index_size_out) thread_args->data->scc_size = thread_args->data->index_size_in;
    else thread_args->data->scc_size = thread_args->data->index_size_out;

    thread_args->data->version++;
    thread_args->data->scc = estimateStronglyConnectedComponents(thread_args->data->index_out, thread_args->data->buffer_out, thread_args->data->scc_size, thread_args->data->version);

    thread_args->data->version++;
    thread_args->data->grail = buildGrailIndex(thread_args->data->index_out, thread_args->data->buffer_out, thread_args->data->scc, thread_args->data->version);
    thread_args->data->version++;

    /*thread_args->data->frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    thread_args->data->frontierB = createQueue();*/  // synoro tou bfs apo ton stoxo pros thn arxh

    /*exploredF = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo thn arxh pros ton stoxo
    exploredB = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo ton stoxo pros thn arxh*/

    pthread_mutex_init(&mutex, 0);
    pthread_mutex_init(&vmutex, 0);
    pthread_cond_init(&cond_start, 0);
    pthread_cond_init(&cond_next, 0);

    pthread_t master_thread;

    //Initialize buffer
    /*thread_args->buffer = malloc(sizeof(Buffer_t));
    thread_args->buffer->start = 0;
    thread_args->buffer->end = -1;
    thread_args->buffer->count = 0;
    thread_args->buffer->size = QUERY_SIZE;
    thread_args->buffer->query = malloc(QUERY_SIZE * sizeof(char *));
    for(i = 0; i < QUERY_SIZE; i++){
        thread_args->buffer->query[i] = malloc(64*sizeof(char));
    }*/
    thread_args->buffer = malloc(sizeof(Buffer));
    thread_args->buffer->first = NULL;
    thread_args->buffer->last = NULL;

    //Thread pool

    int a = 1;

    pthread_create(&master_thread, 0, master_thread_function, thread_args);
    pthread_join(master_thread, 0);

    while(a < thread_args->res_size){

        if(thread_args->results[a] == EMPTY) break;
        printf("%d\n", thread_args->results[a]);
        a++;
    }

    /*fgets(str, sizeof(str), Queries);

    if(strncmp(str, "STATIC", 6) == 0){

        if(index_size_in > index_size_out) scc_size = index_size_in;
        else scc_size = index_size_out;

        version++;
        scc = estimateStronglyConnectedComponents(index_out, buffer_out, scc_size, version);

        version++;
        grail = buildGrailIndex(index_out, buffer_out, scc, version);
        version++;

        fgets(str, sizeof(str), Queries);

        while (!feof(Queries)) {
            if (str[0] == 'Q') {

                toID(str, &N1, &N2);

                if (lookup(index_out, N1, index_size_out) == ALR_EXISTS && lookup(index_in, N2, index_size_in) == ALR_EXISTS && isReachableGrailIndex(grail,N1,N2,scc) == MAYBE) {
                    version++;
                    steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB, version);

                    printf("%d\n", steps);
                } else
                    printf("-1\n");

                version++;
            }

            fgets(str, sizeof(str), Queries);
        }
        fclose(Queries);
    }
    else{
        //to megethos tou cc tha einai osoi einai oi komvoi sinolika diladi
        //to max twn komvwn tou index_in kai index_out
        if(index_size_in > index_size_out) cc_size = index_size_in;
        else cc_size = index_size_out;

        cc = createCCIndex(cc_size, index_in, index_out, buffer_in, buffer_out, index_size_in,index_size_out, explored, version);
        version++;

        cc->u_size = cc->cc_max;
        cc->metricVal = METRIC;

        initUpdateIndex(cc);

        //fgets(str, sizeof(str), Queries);

        while (!feof(Queries)) {

            if (str[0] == 'A') {

                toID(str, &N1, &N2);

                if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
                    insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out, &available_out);

                if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
                    insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, &available_in);

                addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, &available_out);

                addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, &available_in);

                refreshUpdateIndex(cc, N1, N2);

            } else if (str[0] == 'Q') {

                toID(str, &N1, &N2);

                if (lookup(index_out, N1, index_size_out) == ALR_EXISTS && lookup(index_in, N2, index_size_in) == ALR_EXISTS *//*&& (cc->cc_index[N1] == cc->cc_index[N2] || searchUpdateIndex(*cc,N1,N2,exploredA, version) == FOUND)*//*) {
                    if(cc->cc_index[N1] == cc->cc_index[N2]){
                        version++;
                        steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB, version);
                        printf("%d\n", steps);
                    }
                    else{
                        if(searchUpdateIndex(*cc,N1,N2,explored, version) == FOUND){
                            version++;
                            steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB, version);
                            printf("%d\n", steps);

                            version++;
                            cc->metricVal--;
                            if(cc->metricVal == 0){
                                if(index_size_in > index_size_out) cc_size = index_size_in;
                                else cc_size = index_size_out;
                                cc->cc_max = updateCCIndex(cc,explored, version, cc_size);
                                version++;
                                cc->metricVal = METRIC;
                            }
                        }
                        else{
                            printf("-1\n");
                        }


                        version++;
                    }
                } else
                    printf("-1\n");

            }

            fgets(str, sizeof(str), Queries);
        }
        fclose(Queries);
    }



    if(index_size_in > index_size_out) scc_size = index_size_in;
    else scc_size = index_size_out;*/


    gettimeofday(&tv2, NULL);
    printf("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));


/*    empty(thread_args->data->frontierF);
    empty(thread_args->data->frontierB);*/
    delete(thread_args->data->explored, HT_BIG);
/*    delete(exploredB, HT_BIG);
    delete(exploredA, HT_BIG);*/
    destroyBuffer(thread_args->data->buffer_in);
    destroyBuffer(thread_args->data->buffer_out);
    destroyNodeIndex(thread_args->data->index_in, thread_args->data->index_size_in);
    destroyNodeIndex(thread_args->data->index_out, thread_args->data->index_size_out);
    //if(thread_args->data->cc != NULL) destroyCCIndex(cc);
    if(thread_args->data->scc != NULL) destroyStronglyConnectedComponents(thread_args->data->scc);
    if(thread_args->data->grail != NULL) destroyGrailIndex(thread_args->data->grail);

    return 0;
}

