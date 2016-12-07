#include "scc.h"

#define MAYBE 45
#define NO 46

typedef struct GrailIndex_t{
    //ind *hyper_index_in;
    ind *hyper_index_out;
    list_node *hyper_buffer_out;
   // list_node *hyper_buffer_in;
    uint32_t buf_size_in;
    uint32_t buf_size_out;
    uint32_t ind_size_in;
    uint32_t ind_size_out;
}GrailIndex;


GrailIndex* buildGrailIndex(ind *, list_node * , ind *, list_node *, SCC*, ht_Node *, uint32_t);
int isReachableGrailIndex(GrailIndex*, uint32_t, uint32_t, SCC*);
void destroyGrailIndex(GrailIndex*);

//int bBFS(ind *, ind *, list_node *, list_node *, uint32_t, uint32_t, Queue *, Queue *, ht_Node *, ht_Node *);
