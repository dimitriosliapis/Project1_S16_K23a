#include "scc.h"

#define MAYBE 45
#define NO 46


typedef struct GrailIndex_t{
    ind *hyper_index_out;
    list_node *hyper_buffer_out;
    int buf_size_out;
    int ind_size_out;
}GrailIndex;

GrailIndex* buildGrailIndex(ind *, list_node *, SCC*, int, int);
int isReachableGrailIndex(GrailIndex*, int, int, SCC*);
void destroyGrailIndex(GrailIndex*);