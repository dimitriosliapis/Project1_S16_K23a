#ifndef GRAIL_H
#define GRAIL_H

#include "scc.h"

#define MAYBE 45
#define NO 46
#define YES 47

typedef struct GrailIndex_t {
    ind *hyper_index_out;
    list_node *hyper_buffer_out;
    uint32_t buf_size_out;
    uint32_t ind_size_out;
} GrailIndex;

GrailIndex *buildGrailIndex(ind *, list_node *, SCC *, uint32_t);

int isReachableGrailIndex(GrailIndex *, uint32_t, uint32_t, SCC *);

void destroyGrailIndex(GrailIndex *);

#endif
