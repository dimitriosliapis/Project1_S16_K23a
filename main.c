#include <stdio.h>
#include "index.h"

uint32_t buffer_size = BUFF_SIZE;
uint32_t index_size = IND_SIZE;

int main() {

    int err = 1;
    uint32_t neigh[] = {2,3,4,5,6,7,8,9};
    list_node *buffer_in = NULL, *buffer_out = NULL;
    list_node **index_in = NULL, **index_out = NULL;

    buffer_in = createBuffer();
    index_in = createNodeIndex();

    err = insertNode(index_in, 1, neigh, buffer_in);

    if(err < 0) printf("%d\n",err);
   /* if(buffer_in == NULL) printf("SKATA\n");
    printf("%d,%d\n",buffer_in[0].neighbor[0],buffer_in[BUFF_SIZE-1].neighbor[0]);
    destroyBuffer(&buffer_in);*/
    return 0;
}