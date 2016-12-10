#include "grail.h"

GrailIndex* buildGrailIndex(ind *index_out, list_node *buffer_out, ind *index_in, list_node *buffer_in, SCC* scc, ht_Node *explored, uint32_t version){

    uint32_t i = 0, j = 0, k = 0, v = 0, w = 0;
    ptrdiff_t available_in = 0;
    ptrdiff_t available_out = 0;
    ptrdiff_t offset_in;
    ptrdiff_t offset_out;
    list_node *neighbors_in, *neighbors_out;
    uint32_t neigh_scc = 0;
    uint32_t rank = 1, min_rank = 1;
    Stack dfs_stack;
    Stack stack;

    dfs_stack.last = NULL;
    stack.last = NULL;

    GrailIndex *grail = malloc(sizeof(GrailIndex));

    grail->buf_size_in = scc->components_count;
    grail->buf_size_out = scc->components_count;

    grail->ind_size_in = scc->components_count;
    grail->ind_size_out = scc->components_count;

    /*grail->hyper_buffer_in = createBuffer(grail->buf_size_in);
    grail->hyper_index_in = createNodeIndex(grail->ind_size_in);*/
    grail->hyper_buffer_out = createBuffer(grail->buf_size_out);
    grail->hyper_index_out = createNodeIndex(grail->ind_size_out);





    for(i = 0; i < scc->components_count; i++){
        insertNode(&grail->hyper_index_out, i, &grail->hyper_buffer_out, &scc->components_count, &grail->buf_size_out, &available_out);
        //insertNode(&grail->hyper_index_in, i, &grail->hyper_buffer_in, &scc->components_count, &grail->buf_size_in, &available_in);
    }

    for(i = 0; i < scc->components_count; i++){

        for(j = 0; j < scc->components[i].included_nodes_count; j++){

            offset_out = getListHead(index_out, scc->components[i].included_node_ids[j]);
            if(offset_out >= 0) {                                                                       //DEBUG
                neighbors_out = buffer_out + offset_out;

                k = 0;
                while (k < N) {
                    if (neighbors_out->neighbor[k] == DEFAULT) break;

                    neigh_scc = scc->id_belongs_to_component[neighbors_out->neighbor[k]];

                    if (neigh_scc != i) {
                        addEdge(&grail->hyper_index_out, i, neigh_scc, &grail->hyper_buffer_out, &grail->buf_size_out,
                                &available_out);
                    }

                    k++;
                    if (k == N) {
                        if (neighbors_out->nextListNode > 0) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;  // an uparxei sunexizei se auton
                            k = 0;
                        }
                    }
                }
            }


            /*offset_in = getListHead(index_in, scc->components[i].included_node_ids[j]);

            if(offset_in >= 0) {
                neighbors_in = buffer_in + offset_in;

                k = 0;
                while (k < N) {
                    if (neighbors_in->neighbor[k] == DEFAULT) break;

                    neigh_scc = scc->id_belongs_to_component[neighbors_in->neighbor[k]];

                    if (neigh_scc != i) {
                        addEdge(&grail->hyper_index_in, i, neigh_scc, &grail->hyper_buffer_in, &grail->buf_size_in,
                                &available_in);
                    }

                    k++;
                    if (k == N) {
                        if (neighbors_in->nextListNode > 0) {
                            neighbors_in = buffer_in + neighbors_in->nextListNode;  // an uparxei sunexizei se auton
                            k = 0;
                        }
                    }
                }
            }*/
        }

    }

    for(i = 0; i < scc->components_count; i++){

        if (search(explored, i, HT_BIG, version) == FOUND) continue;

        push(&dfs_stack, i);
        push(&stack, i);
        while(!stackIsEmpty(&dfs_stack)) {

            v = peek(&dfs_stack);

            if (search(explored, v, HT_BIG, version) == NOT_FOUND) {
                insert(explored, v, HT_BIG, version);
            }
            else{
                grail->hyper_index_out[v].min_rank = rank;
                grail->hyper_index_out[v].rank = rank;
                rank++;
                if (grail->hyper_index_out[v].num_of_children == 0) pop(&dfs_stack);


            }





            if (grail->hyper_index_out[v].num_of_children != 0) {

                //offset_in = getListHead(grail->hyper_index_in, v);
                offset_out = getListHead(grail->hyper_index_out, v);
                //neighbors_in = grail->hyper_buffer_in + offset_in;
                neighbors_out = grail->hyper_buffer_out + offset_out;

                k = 0;
                while (k < N) {

                    w = neighbors_out->neighbor[k];

                    if (w == DEFAULT) {
                        pop(&dfs_stack);
                        break;
                    }
                    if (search(explored, w, HT_BIG, version) == NOT_FOUND) {
                      //  push(&stack, w);
                        push(&dfs_stack, w);
                        break;
                    }
                    else{
                        grail->hyper_index_out[v].rank = rank;
                        if(grail->hyper_index_out[v].min_rank > grail->hyper_index_out[w].min_rank)
                            grail->hyper_index_out[v].min_rank = grail->hyper_index_out[w].min_rank;
                    }
                    k++;
                    if (k == N && neighbors_out->nextListNode != -1) {
                        neighbors_out = grail->hyper_buffer_out + neighbors_out->nextListNode;
                        k = 0;
                    }
                    else pop(&dfs_stack);

                }
                rank++;
            }

            /*else{
                grail->hyper_index_out[v].min_rank = rank;
                grail->hyper_index_out[v].rank = rank;
                rank++;
            }*/
        }
        /*while (!stackIsEmpty(&stack)) {

            v = pop(&stack);

            //offset_in = getListHead(grail->hyper_index_in, v);
            offset_out = getListHead(grail->hyper_index_out, v);
            //neighbors_in = grail->hyper_buffer_in + offset_in;
            neighbors_out = grail->hyper_buffer_out + offset_out;

            if(offset_out >= 0 && neighbors_out->neighbor[0] != DEFAULT) {
                k = 0;
                min_rank = rank;
                while (k < N) {
                    if (neighbors_out->neighbor[k] == DEFAULT) break;
                    if(min_rank > grail->hyper_index_out[neighbors_out->neighbor[k]].min_rank){
                        min_rank = grail->hyper_index_out[neighbors_out->neighbor[k]].min_rank;
                    }
                    k++;
                    if (k == N && neighbors_out->nextListNode != -1) {
                        neighbors_out = grail->hyper_buffer_out + neighbors_out->nextListNode;
                        i = 0;
                    }
                }
                grail->hyper_index_out[v].rank = rank;
                grail->hyper_index_out[v].min_rank = min_rank;
                rank++;
                insert(explored, v, HT_BIG, version);
            }
            else{
                grail->hyper_index_out[v].rank = rank;
                grail->hyper_index_out[v].min_rank = rank;
                rank++;
                insert(explored, v, HT_BIG, version);
            }

        }*/
    }

    deleteStack(&stack);

    return grail;
}

int isReachableGrailIndex(GrailIndex* index, uint32_t source_node, uint32_t target_node, SCC *scc){

    uint32_t scc_source, scc_target;

    scc_source = scc->id_belongs_to_component[source_node];
    scc_target = scc->id_belongs_to_component[target_node];

    if(scc_source == DEFAULT || scc_target == DEFAULT) return NO;
    if(scc_source == scc_target) return MAYBE;
    if((index->hyper_index_out[scc_source].min_rank <= index->hyper_index_out[scc_target].min_rank) && (index->hyper_index_out[scc_source].rank > index->hyper_index_out[scc_target].rank)) return MAYBE;
    return NO;
}

void destroyGrailIndex(GrailIndex* index){

    //destroyBuffer(index->hyper_buffer_in);
    destroyBuffer(index->hyper_buffer_out);
    //destroyNodeIndex(index->hyper_index_in, index->ind_size_in);
    destroyNodeIndex(index->hyper_index_out, index->ind_size_out);

    free(index);
}