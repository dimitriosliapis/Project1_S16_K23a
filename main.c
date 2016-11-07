#include <stdio.h>
#include <sys/time.h>
#include "search.h"

int toID(char *, uint32_t *, uint32_t *);

int main(int argc, char *argv[]) {

    FILE *Graph = NULL;
    FILE *Queries = NULL;
    list_node *buffer_in = NULL, *buffer_out = NULL;
    ind *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2;
    uint32_t buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE;
    uint32_t index_size_in = IND_SIZE, index_size_out = IND_SIZE;
    ptrdiff_t available_in = 0, available_out = 0;

    // arguments
    if (argc == 3) {
        Graph = fopen(argv[1], "r");
        Queries = fopen(argv[2], "r");
    } else {
        printf("Datasets missing");
        return 0;
    }

    // create pairs of indexes and buffers
    buffer_in = createBuffer(buffer_size_in);
    index_in = createNodeIndex(index_size_in);
    buffer_out = createBuffer(buffer_size_out);
    index_out = createNodeIndex(index_size_out);

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

        addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, index_size_out, &available_out);

        addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, index_size_in, &available_in);

        fgets(str, sizeof(str), Graph);
    }
    fclose(Graph);

    fgets(str, sizeof(str), Queries);

    while (!feof(Queries)) {

        if (str[0] == 'A') {

            toID(str, &N1, &N2);

            if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
                insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out, &available_out);

            if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
                insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, &available_in);

            addEdge(&index_out, N1, N2, &buffer_out, &buffer_size_out, index_size_out, &available_out);

            addEdge(&index_in, N2, N1, &buffer_in, &buffer_size_in, index_size_in, &available_in);

        } else if (str[0] == 'Q') {

            toID(str, &N1, &N2);

            if (lookup(index_out, N1, index_size_out) == ALR_EXISTS &&
                lookup(index_in, N2, index_size_in) == ALR_EXISTS) {
                printf("%d\n", bBFS(index_in, index_out, buffer_in, buffer_out, N1, N2));
                for (int i = 0; i < index_size_out; i++) {
                    index_out[i].visited = 0;
                    index_out[i].steps = 0;
                }
                for (int i = 0; i < index_size_in; i++) {
                    index_in[i].visited = 0;
                    index_in[i].steps = 0;
                }
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
