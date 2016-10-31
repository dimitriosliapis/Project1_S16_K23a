#include <stdio.h>
#include "index.h"
#include "string.h"



int main(int argc, char *argv[]) {

    FILE *Graph = NULL;
    FILE *Queries = NULL;
    list_node *buffer_in = NULL, *buffer_out = NULL;
    ptrdiff_t *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2;
    uint32_t buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE;
    uint32_t index_size_in = IND_SIZE, index_size_out = IND_SIZE;

    buffer_in = createBuffer(buffer_size_in);
    index_in = createNodeIndex(index_size_in);
    buffer_out = createBuffer(buffer_size_out);
    index_out = createNodeIndex(index_size_out);

    if (argc == 3) {
        Graph = fopen(argv[1], "r");
        Queries = fopen(argv[2], "r");
    } else {
        printf("Datasets missing");
        return 0;
    }

    char str[64];
    char *argument;
    fgets(str, sizeof(str), Graph);
    argument = strtok(str, " \n");

    while (strcmp(argument, "S") != 0) {

        N1 = atoi(argument);
        argument = strtok(NULL, " \n");
        N2 = atoi(argument);


        if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
            insertNode(index_out, N1, buffer_out, &index_size_out, &buffer_size_out);
        if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
            insertNode(index_in, N2, buffer_in, &index_size_in, &buffer_size_in);
        addEdge(index_out, N1, N2, buffer_out,&buffer_size_out, index_size_out);
        addEdge(index_in, N2, N1, buffer_in, &buffer_size_in,index_size_in);

        fgets(str, sizeof(str), Graph);
        argument = strtok(str, " \n");
    }

    return 0;
}
