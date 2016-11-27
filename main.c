#include <sys/time.h>
#include "cc.h"

int toID(char *, uint32_t *, uint32_t *);

int main(int argc, char *argv[]) {

    FILE *Graph = NULL, *Queries = NULL;
    list_node *buffer_in = NULL, *buffer_out = NULL;
    ind *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2, buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE, index_size_in = IND_SIZE, index_size_out = IND_SIZE;
    ptrdiff_t available_in = 0, available_out = 0;
    Queue *frontierF = NULL, *frontierB = NULL;
    ht_Node *exploredF = NULL, *exploredB = NULL, *explored = NULL;
    int steps = 0;



    uint32_t *cc_index = NULL;
    uint32_t cc_size = 0;
    uint32_t cc_max = 0;

    u_node *update_index = NULL;
    uint32_t update_index_size = 0;

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

    if(index_size_in > index_size_out) cc_size = index_size_in;
    else cc_size = index_size_out;

    cc_index = malloc(sizeof(uint32_t) * cc_size);
    cc_max = createCCIndex(&cc_index, cc_size, index_in, index_out, buffer_in, buffer_out, index_size_in,index_size_out, explored);

    update_index_size = cc_max;

    update_index = malloc(update_index_size * sizeof(u_node));

    uint32_t a = 0;
    while(a < update_index_size) {
        update_index[a].cc_array = NULL;
        update_index[a].size = 0;
        update_index[a].state = 'o';//empty
        a++;
    }

    frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    frontierB = createQueue();  // synoro tou bfs apo ton stoxo pros thn arxh

    exploredF = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo thn arxh pros ton stoxo
    exploredB = createHashtable(HT_BIG);  // komvoi pou exei episkeftei o bfs apo ton stoxo pros thn arxh

    fgets(str, sizeof(str), Queries);

    while (!feof(Queries)) {

        if (str[0] == 'A') {

            toID(str, &N1, &N2);

            if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
                insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out, &available_out);

            if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
                insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, &available_in);

            addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, &available_out);

            addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, &available_in);

            refreshUpdateIndex(cc_index, &update_index, &update_index_size, N1, N2);

        } else if (str[0] == 'Q') {

            toID(str, &N1, &N2);

            if (lookup(index_out, N1, index_size_out) == ALR_EXISTS && lookup(index_in, N2, index_size_in) == ALR_EXISTS && ((N1 < cc_size && N2 < cc_size && cc_index[N1] == cc_index[N2] && cc_index[N1] != DEFAULT) || searchUpdateIndex(cc_index,update_index,N1,N2,explored) == FOUND)) {

/*                if(N1 == 212969 && N2 == 233631){
                    printf("helloooooo\n");
                }*/
                steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB, exploredF, exploredB);
/*                if(steps == 3){
                    steps = bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2, frontierF, frontierB, exploredF, exploredB);
                }*/
                printf("%d\n", steps);
            } else
                printf("-1\n");
        }

        fgets(str, sizeof(str), Queries);
    }
    fclose(Queries);


    gettimeofday(&tv2, NULL);
    printf("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

    empty(frontierF);
    empty(frontierB);
    delete(exploredF, HT_BIG);
    delete(exploredB, HT_BIG);
    delete(explored, HT_BIG);
    destroyBuffer(buffer_in);
    destroyBuffer(buffer_out);
    destroyNodeIndex(index_in, index_size_in);
    destroyNodeIndex(index_out, index_size_out);
    free(cc_index);
    for(a = 0; a < update_index_size; a++ ){
        free(update_index[a].cc_array);
    }
    free(update_index);

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