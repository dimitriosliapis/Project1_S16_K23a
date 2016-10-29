#include <stdio.h>
#include "index.h"
#include "string.h"

uint32_t buffer_size = BUFF_SIZE;
uint32_t index_size = IND_SIZE;

int main(int argc, char *argv[]) {

    FILE *Graph = NULL;
    FILE *Queries = NULL;
    list_node *buffer_in = NULL, *buffer_out = NULL;
    ptrdiff_t *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2;

    buffer_in = createBuffer();
    index_in = createNodeIndex();
    buffer_out = createBuffer();
    index_out = createNodeIndex();

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

        if (lookup(index_out, N1) == NOT_EXIST)
            insertNode(index_out, N1, buffer_out);
        addEdge(index_out, N1, N2, buffer_out);

        if (lookup(index_in, N2) == NOT_EXIST)
            insertNode(index_in, N2, buffer_in);
        addEdge(index_in, N2, N1, buffer_in);

        fgets(str, sizeof(str), Graph);
        argument = strtok(str, " \n");
    }

    return 0;
}
