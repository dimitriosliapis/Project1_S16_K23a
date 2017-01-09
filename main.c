#include <sys/time.h>
#include "grail.h"
#include "thread.h"

int toID(char *, uint32_t *, uint32_t *);

Buffer_t buffer;

int main(int argc, char *argv[]) {

    FILE *Graph = NULL, *Queries = NULL;
    list_node *buffer_in = NULL, *buffer_out = NULL;
    ind *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2, buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE, index_size_in = IND_SIZE, index_size_out = IND_SIZE;
    ptrdiff_t available_in = 0, available_out = 0;
    Queue *frontierF = NULL, *frontierB = NULL;
    ht_Node *explored = NULL;
    uint32_t version = 0;
    int steps = 0;

//thread
    CC *cc = NULL;
    uint32_t cc_size = 0;
    uint32_t scc_size = 0;

    SCC *scc = NULL;
    GrailIndex *grail = NULL;

    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);

    pthread_t master_thread;

    //Initialize buffer
    buffer.start = 0;
    buffer.end = -1;
    buffer.count = 0;
    buffer.querry = (char *)malloc(QUERY_SIZE*sizeof(char));

    //Thread pool
    pthread_t *worker_threads = (pthread_t *)malloc(THREAD_POOL_SIZE*sizeof(pthread_t));

    int a = 0;
    for(a = 0 ; a < 5 ; a++) pthread_create(&worker_threads[a], 0, worker_function, 0);

    pthread_create(&master_thread, 0, master_thread_function, 0);
    pthread_join(master_thread, 0);

    // orismata
    if (argc == 3) {
        Graph = fopen(argv[1], "r");
        Queries = fopen(argv[2], "r");
    } else {
        printf("Datasets missing");
        return 0;
    }

    // zeugh indexes kai buffers
    buffer_in = createBuffer(buffer_size_in);
    index_in = createNodeIndex(index_size_in);
    buffer_out = createBuffer(buffer_size_out);
    index_out = createNodeIndex(index_size_out);

    explored = createHashtable(HT_BIG);


    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    char str[64];
    fgets(str, sizeof(str), Graph);



    while (str[0] != 'S') {

        toID(str, &N1, &N2);

        if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
            insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out, &available_out);

        if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
            insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, &available_in);

        addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, &available_out);

        addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, &available_in);

        fgets(str, sizeof(str), Graph);
    }

    fclose(Graph);

    frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    frontierB = createQueue();  // synoro tou bfs apo ton stoxo pros thn arxh

    /*exploredF = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo thn arxh pros ton stoxo
    exploredB = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo ton stoxo pros thn arxh*/

    fgets(str, sizeof(str), Queries);

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

                if (lookup(index_out, N1, index_size_out) == ALR_EXISTS && lookup(index_in, N2, index_size_in) == ALR_EXISTS /*&& (cc->cc_index[N1] == cc->cc_index[N2] || searchUpdateIndex(*cc,N1,N2,exploredA, version) == FOUND)*/) {
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
    else scc_size = index_size_out;


    gettimeofday(&tv2, NULL);
    printf("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));


    empty(frontierF);
    empty(frontierB);
    delete(explored, HT_BIG);
/*    delete(exploredB, HT_BIG);
    delete(exploredA, HT_BIG);*/
    destroyBuffer(buffer_in);
    destroyBuffer(buffer_out);
    destroyNodeIndex(index_in, index_size_in);
    destroyNodeIndex(index_out, index_size_out);
    if(cc != NULL) destroyCCIndex(cc);
    if(scc != NULL) destroyStronglyConnectedComponents(scc);
    if(grail != NULL) destroyGrailIndex(grail);

    return 0;
}

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