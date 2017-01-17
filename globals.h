#ifndef PROJECT1_S16_K23A_GLOBALS_H
#define PROJECT1_S16_K23A_GLOBALS_H

#include "grail.h"


list_node *global_buffer_in, *global_buffer_out;
ind *global_index_in, *global_index_out;
int  global_buffer_size_in, global_buffer_size_out, global_index_size_in, global_index_size_out;
ptrdiff_t global_available_in, global_available_out;
Queue *global_frontierF, *global_frontierB;
ht_Node *global_explored;
int global_version;
int global_steps;
int global_scc_size;

SCC *global_scc;
GrailIndex *global_grail;

#endif //PROJECT1_S16_K23A_GLOBALS_H
