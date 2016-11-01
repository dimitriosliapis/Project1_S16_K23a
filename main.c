#include <stdio.h>
#include "index.h"
#include "string.h"

uint32_t toID(char*, int*, int*);

int main(int argc, char *argv[]) {

    FILE *Graph = NULL;
    FILE *Queries = NULL;
    list_node *buffer_in = NULL, *buffer_out = NULL;
    ptrdiff_t *index_in = NULL, *index_out = NULL;
    uint32_t N1, N2;
    uint32_t buffer_size_in = BUFF_SIZE, buffer_size_out = BUFF_SIZE;
    uint32_t index_size_in = IND_SIZE, index_size_out = IND_SIZE;
    ptrdiff_t prev_in = 0, prev_out = 0;

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
    fgets(str, sizeof(str), Graph);

    while (strcmp(str, "S") != 0) {

        toID(str, &N1, &N2);

        if (lookup(index_out, N1, index_size_out) == NOT_EXIST)
            prev_out = insertNode(&index_out, N1, &buffer_out, &index_size_out, &buffer_size_out,prev_out);
        if (lookup(index_in, N2, index_size_in) == NOT_EXIST)
            prev_in = insertNode(&index_in, N2, &buffer_in, &index_size_in, &buffer_size_in, prev_in);
        prev_out = addEdge(index_out, N1, N2, &buffer_out,&buffer_size_out, index_size_out, prev_out);
        prev_in = addEdge(index_in, N2, N1, &buffer_in, &buffer_size_in,index_size_in, prev_in);

        fgets(str, sizeof(str), Graph);
    }

    return 0;
}

uint32_t toID(char *str, int *N1, int *N2){

    uint32_t i, j, id;

    if(str == NULL) return 0;

    i = 0;
    for(j = 0; j < 2; j++) {
        id = 0;
        while ((str[i] >= '0') && (str[i] <= '9')) {
            id = 10 * id + (str[i] - '0');
            i++;
        }
        if(j == 0) *N1 = id;
        else *N2 = id;
        i++;
    }


    return id;
}
