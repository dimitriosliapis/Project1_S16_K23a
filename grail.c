#include "grail.h"
#include <sys/time.h>

GrailIndex* buildGrailIndex(ind *index_out, list_node *buffer_out, SCC* scc, int version, int thread_id){

    int i = 0, j = 0, k = 0, v = 0, w = 0;
    ptrdiff_t available_out = 0;
    ptrdiff_t offset_out;
    list_node *neighbors_out;
    int neigh_scc = 0;
    int rank = 1, min_rank = 1;
    Stack dfs_stack;
    struct timeval tv1, tv2;

    gettimeofday(&tv1, NULL);


    dfs_stack.last = NULL;

    GrailIndex *grail = malloc(sizeof(GrailIndex));

    grail->buf_size_out = scc->components_count;
    grail->ind_size_out = scc->components_count;

    grail->hyper_buffer_out = createBuffer(grail->buf_size_out);
    grail->hyper_index_out = createNodeIndex(grail->ind_size_out);


    //eisagwgi olwn twn SCC sto hyper_index
    for(i = 0; i < scc->components_count; i++){
        insertNode(&grail->hyper_index_out, i, &grail->hyper_buffer_out, &scc->components_count, &grail->buf_size_out, &available_out);
    }

    //prosthiki akmwn metaksi tous
    for(i = 0; i < scc->components_count; i++){

        for(j = 0; j < scc->components[i].included_nodes_count; j++){

            offset_out = getListHead(index_out, scc->components[i].included_node_ids[j]);
            if(offset_out >= 0) {
                neighbors_out = buffer_out + offset_out;

                k = 0;
                while (k < N) {
                    if (neighbors_out->neighbor[k] == DEFAULT) break;

                    neigh_scc = scc->id_belongs_to_component[neighbors_out->neighbor[k]];

                    if (neigh_scc != i) {
                        addEdge(&grail->hyper_index_out, i, neigh_scc, &grail->hyper_buffer_out, &grail->buf_size_out,
                                &available_out, thread_id);
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
        }
    }


    //algorithmos GRAIL
    for(i = 0; i < scc->components_count; i++){

        //if (search(explored, i, HT_BIG, version) == FOUND) continue;
        //an einai visited sinexise sto epomeno
        if(grail->hyper_index_out[i].visited[thread_id] == version) continue;

        push(&dfs_stack, i);
        while(!stackIsEmpty(&dfs_stack)) {

            v = peek(&dfs_stack);

            //dimiourgia hashtable gia na tsekarei an exei perasei apo ola ta paidia tou
            if(grail->hyper_index_out[v].neighbors == NULL){
                grail->hyper_index_out[v].neighbors = createHashtable(HT_SMALL);
            }

            //an den exei paidia tote v.rank = rank, v.min_rank = rank kai pop apo tin stoiva
            if (grail->hyper_index_out[v].num_of_children == 0) {
                rank++;
                grail->hyper_index_out[v].min_rank = rank;
                grail->hyper_index_out[v].rank = rank;
                pop(&dfs_stack);
                //insert(explored, v, HT_BIG, version);
                grail->hyper_index_out[v].visited[thread_id] = version;
            }
            else {

                offset_out = getListHead(grail->hyper_index_out, v);
                neighbors_out = grail->hyper_buffer_out + offset_out;

                k = 0;
                while (k < N) {

                    w = neighbors_out->neighbor[k];

                    //den exei allo paidi
                    if (w == DEFAULT) break;
                    //if (search(explored, w, HT_BIG, version) == NOT_FOUND) {

                    //an den einai visited to paidi push kai break gia na ginei DFS sta paidia tou
                    if(grail->hyper_index_out[w].visited[thread_id] != version) {
                        push(&dfs_stack, w);
                        k++;
                        continue;
                    }
                    else if (search(grail->hyper_index_out[v].neighbors, w, HT_SMALL, version, thread_id) == NOT_FOUND) { //alliws an einai visited
                        // tin prwti fora pou tha ksanaperasei tha auksithei o metritis all_children

                        grail->hyper_index_out[v].all_children_in_scc++;
                        insert(grail->hyper_index_out[v].neighbors, w, HT_SMALL, version, thread_id);

                        //pairnei to min_rank apo kathe paidi pou exei termatisei
                        if (grail->hyper_index_out[v].min_rank > grail->hyper_index_out[w].min_rank)
                            grail->hyper_index_out[v].min_rank = grail->hyper_index_out[w].min_rank;
                    }
                    k++;
                    if (k == N){
                        if(neighbors_out->nextListNode != -1) {
                            neighbors_out = grail->hyper_buffer_out + neighbors_out->nextListNode;
                            k = 0;
                        }
                    }
                }
                //an exoume episkeutei ola ta paidia tote ftianoume kai to rank tou patera kai ton kanoume pop
                if (grail->hyper_index_out[v].num_of_children != 0 && grail->hyper_index_out[v].all_children_in_scc == grail->hyper_index_out[v].num_of_children) {
                    rank++;
                    grail->hyper_index_out[v].rank = rank;
                    //insert(explored, v, HT_BIG, version);
                    grail->hyper_index_out[v].visited[thread_id] = version;
                    pop(&dfs_stack);
                }
            }
        }
    }
    gettimeofday(&tv2, NULL);

    printf("Grail time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));
    //rank = 0;
    //deletestack(dfs_stack);

    return grail;
}

//sinartisi pou epistrefei tin pithanotita na uparxei monopati ap ton source ston target
int isReachableGrailIndex(GrailIndex* index, int source_node, int target_node, SCC *scc){

    int scc_source, scc_target;

    scc_source = scc->id_belongs_to_component[source_node];
    scc_target = scc->id_belongs_to_component[target_node];

    if(scc_source == DEFAULT || scc_target == DEFAULT) return NO;
    if(scc_source == scc_target) return MAYBE;
    if((index->hyper_index_out[scc_source].min_rank <= index->hyper_index_out[scc_target].min_rank) && (index->hyper_index_out[scc_source].rank > index->hyper_index_out[scc_target].rank)) return MAYBE;
    return NO;
}
// free GrailIndex
void destroyGrailIndex(GrailIndex* index){

    destroyBuffer(index->hyper_buffer_out);
    destroyNodeIndex(index->hyper_index_out, index->ind_size_out);

    free(index);
}