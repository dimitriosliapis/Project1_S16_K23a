#include <sys/time.h>
#include "thread.h"


arg *thread_args;




int main(int argc, char *argv[]) {

    FILE *Graph = NULL;//, *Queries = NULL;
    //list_node *buffer_in = NULL, *buffer_out = NULL;
    //ind *index_in = NULL, *index_out = NULL;
    int N1, N2;//, buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE, index_size_in = IND_SIZE, index_size_out = IND_SIZE;
    //ptrdiff_t available_in = 0, available_out = 0;
    //Queue *frontierF = NULL, *frontierB = NULL;
    //ht_Node *explored = NULL;
    //int version = 0;
    //int steps = 0;

//thread
   /* CC *cc = NULL;
    int cc_size = 0;
    int scc_size = 0;

    SCC *scc = NULL;
    GrailIndex *grail = NULL;*/

    int i = 0;
    struct timeval tv1, tv2;
    char str[64];
    int a = 1;


    gettimeofday(&tv1, NULL);

    thread_args = malloc(sizeof(arg));
    //thread_args->data = malloc(sizeof(work_data));

    global_buffer_size_in = BUFF_SIZE;
    global_buffer_size_out = BUFF_SIZE;
    global_index_size_in = IND_SIZE;
    global_index_size_out = IND_SIZE;

    global_available_in = 0;
    global_available_out = 0;


    // orismata
    if (argc == 3) {
        Graph = fopen(argv[1], "r");
        thread_args->file = fopen(argv[2], "r");
    } else {
        printf("Datasets missing");
        return 0;
    }

    static_graph = 0;
    fgets(str, sizeof(str), thread_args->file);

    if(strncmp(str, "STATIC", 6) == 0) static_graph = 1;

    // zeugh indexes kai buffers
    global_buffer_in = createBuffer(global_buffer_size_in);
    global_index_in = createNodeIndex(global_index_size_in);
    global_buffer_out = createBuffer(global_buffer_size_out);
    global_index_out = createNodeIndex(global_index_size_out);

    global_explored = createHashtable(HT_BIG);




    fgets(str, sizeof(str), Graph);



    while (str[0] != 'S') {

        toID(str, &N1, &N2);

        if (lookup(global_index_out, N1, global_index_size_out) == NOT_EXIST) {
            insertNode(&global_index_out, N1, &global_buffer_out, &global_index_size_out, &global_buffer_size_out,
                       &global_available_out);
            if(max < N1) max = N1;
        }
        if (lookup(global_index_in, N2, global_index_size_in) == NOT_EXIST) {
            insertNode(&global_index_in, N2, &global_buffer_in, &global_index_size_in, &global_buffer_size_in,
                       &global_available_in);
            if(max < N2) max = N2;
        }
        addEdge(&global_index_out, N1, N2, &global_buffer_out, &global_buffer_size_out, &global_available_out, 0);

        addEdge(&global_index_in, N2, N1, &global_buffer_in, &global_buffer_size_in, &global_available_in, 0);

        fgets(str, sizeof(str), Graph);
    }

    fclose(Graph);

    freeNeighbors(global_index_in, max);
    freeNeighbors(global_index_out, max);

    pthread_mutex_init(&mutex, NULL);
    //pthread_mutex_init(&mutexb, NULL);
    pthread_mutex_init(&vmutex, NULL);
    pthread_cond_init(&cond_start, NULL);
    pthread_cond_init(&cond_next, NULL);

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

    thread_args->res_size = RES_INIT;
    thread_args->results = malloc(RES_INIT*sizeof(int));
    for(i = 0; i < thread_args->res_size; i++) thread_args->results[i] = EMPTY;



    if(static_graph){
        global_scc_size = max;

        global_version++;
        global_scc = estimateStronglyConnectedComponents(global_index_out, global_buffer_out, global_scc_size, global_version, 0);


        global_version = 0;
        global_version++;
        global_grail = buildGrailIndex(global_index_out, global_buffer_out, global_scc, global_version, 0);
        global_version++;

        pthread_create(&master_thread, NULL, master_thread_function, thread_args);
        pthread_join(master_thread, NULL);
    }
    else{
        rewind(thread_args->file);

        pthread_create(&master_thread, NULL, master_thread_function_dynamic, thread_args);
        pthread_join(master_thread, NULL);
    }
    /*if(global_index_size_in > global_index_size_out) global_scc_size = global_index_size_in;
    else global_scc_size = global_index_size_out;*/



    /*thread_args->data->frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    thread_args->data->frontierB = createQueue();*/  // synoro tou bfs apo ton stoxo pros thn arxh

    /*exploredF = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo thn arxh pros ton stoxo
    exploredB = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo ton stoxo pros thn arxh*/





    //Thread pool



    while(a < thread_args->res_size){

        /*if(thread_args->results[a] == EMPTY){
            a++;
            continue;
        }*/
        printf("%d\n", thread_args->results[a]);
        a++;
    }

    gettimeofday(&tv2, NULL);

    printf("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

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





/*    empty(thread_args->data->frontierF);
    empty(thread_args->data->frontierB);*/
    delete(global_explored, HT_BIG);
/*    delete(exploredB, HT_BIG);
    delete(exploredA, HT_BIG);*/
    destroyBuffer(global_buffer_in);
    destroyBuffer(global_buffer_out);
    destroyNodeIndex(global_index_in, global_index_size_in);
    destroyNodeIndex(global_index_out,global_index_size_out);
    //if(thread_args->data->cc != NULL) destroyCCIndex(cc);
    if(global_scc != NULL) destroyStronglyConnectedComponents(global_scc);
    if(global_grail != NULL) destroyGrailIndex(global_grail);
    free(thread_args->results);
    free(thread_args->buffer);
    //free(thread_args->data);

    return 0;
}

