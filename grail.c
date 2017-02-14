#include <sys/time.h>
#include "grail.h"

extern struct timeval tv1, tv2;

void shuffle(uint32_t *array, uint32_t size) {

    uint32_t i = 0, t = 0, j = 0;

    srand((unsigned)time(NULL));

    for(i = 0 ; i < size ; i++) {
        j = i + rand() / (RAND_MAX / (size - i) + 1);
        t = array[j];
        array[j] = array[i];
        array[i] = t;
    }

    return;
}

GrailIndex *buildGrailIndex(ind *index_out, list_node *buffer_out, SCC *scc, uint32_t version) {

    uint32_t i = 0, j = 0, k = 0, v = 0, w = 0,
            neigh_scc = 0,
            rank = 1;
    ptrdiff_t available_out = 0,
            offset_out;
    list_node *neighbors_out;
    Stack_t *dfs_stack = createStack();
    int *scc_con;

    GrailIndex *grail = malloc(sizeof(GrailIndex));

    grail->buf_size_out = scc->components_count;
    grail->ind_size_out = scc->components_count;

    grail->hyper_buffer_out = createBuffer(grail->buf_size_out);
    grail->hyper_index_out = createNodeIndex(grail->ind_size_out, 1);

    // eisagwgi olwn twn SCC sto hyper_index
    for (i = 0; i < scc->components_count; i++)
        insertNode(&grail->hyper_index_out, i, &grail->hyper_buffer_out, &scc->components_count, &grail->buf_size_out,
                   &available_out, 1);

    scc_con = malloc(scc->components_count * sizeof(int));
    for (i = 0; i < scc->components_count; i++)
        scc_con[i] = -1;

    // prosthiki akmwn metaksi tous
    for (i = 0; i < scc->components_count; i++) {

        for (j = 0; j < scc->components[i].included_nodes_count; j++) {

            offset_out = getListHead(index_out, scc->components[i].included_node_ids[j]);
            if (offset_out != -1) {
                neighbors_out = buffer_out + offset_out;

                k = 0;
                while (k < N) {
                    if (neighbors_out->neighbor[k] == DEFAULT) break;

                    neigh_scc = scc->id_belongs_to_component[neighbors_out->neighbor[k]];

                    if (neigh_scc != i && scc_con[neigh_scc] != i) {
                        addEdge(&grail->hyper_index_out, i, neigh_scc, &grail->hyper_buffer_out, &grail->buf_size_out,
                                &available_out, 0, 0);
                        scc_con[neigh_scc] = i;
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

    free(scc_con);

    gettimeofday(&tv2, NULL);
    printf("%f sec: Hypergraph creation\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

    uint32_t *array = malloc(grail->ind_size_out*sizeof(uint32_t));
    for(i = 0 ; i < grail->ind_size_out ; i++) array[i] = i;

    uint32_t a;

    for (j = 0 ; j < NUM_GRAIL ; j++) {

        rank = 0;

        // algorithmos GRAIL
        for (i = 0 ; i < scc->components_count ; i++) {

            a = array[i];

            // an einai visited sinexise sto epomeno
            if (grail->hyper_index_out[a].visited[0] == version)
                continue;
            else
                pushinstack(dfs_stack, a);// tin prwti fora pou tha ksanaperasei tha auksithei o metritis all_children

            while (!stackisempty(dfs_stack)) {

                v = peekfromstack(dfs_stack);

                // an exoume episkeutei ola ta paidia tote ftianoume kai to rank tou patera kai ton kanoume pop
                if ((grail->hyper_index_out[v].num_of_children != 0) && (grail->hyper_index_out[v].s_data->all_children_in_scc[j] == grail->hyper_index_out[v].num_of_children)) {
                    rank++;
                    grail->hyper_index_out[v].s_data->rank[j] = rank;
                    grail->hyper_index_out[v].visited[0] = version;
                    popfromstack(dfs_stack);
                }

                // an den exei paidia tote v.rank = rank, v.min_rank = rank kai pop apo tin stoiva
                if (grail->hyper_index_out[v].num_of_children == 0) {
                    rank++;
                    grail->hyper_index_out[v].s_data->min_rank[j] = rank;
                    grail->hyper_index_out[v].s_data->rank[j] = rank;
                    grail->hyper_index_out[v].visited[0] = version;
                    popfromstack(dfs_stack);
                }

                // an exei paidia
                if (grail->hyper_index_out[v].num_of_children != 0) {

                    offset_out = getListHead(grail->hyper_index_out, v);
                    neighbors_out = grail->hyper_buffer_out + offset_out;

                    k = 0;
                    while (k < N) {

                        w = neighbors_out->neighbor[k];

                        // den exei allo paidi
                        if (w == DEFAULT) break;

                        // an den einai visited to paidi push kai break gia na ginei DFS sta paidia tou
                        if (grail->hyper_index_out[w].visited[0] != version) {
                            pushinstack(dfs_stack, w);
                        } else { // alliws an einai visited

                            // pairnei to min_rank apo kathe paidi pou exei termatisei
                            if (grail->hyper_index_out[v].s_data->min_rank[j] > grail->hyper_index_out[w].s_data->min_rank[j])
                                grail->hyper_index_out[v].s_data->min_rank[j] = grail->hyper_index_out[w].s_data->min_rank[j];

                            grail->hyper_index_out[v].s_data->all_children_in_scc[j] = grail->hyper_index_out[v].num_of_children;
                        }

                        k++;
                        if (k == N) {
                            if (neighbors_out->nextListNode != -1) {
                                neighbors_out = grail->hyper_buffer_out + neighbors_out->nextListNode;
                                k = 0;
                            }
                        }
                    }
                }
            }
        }

        shuffle(array, grail->ind_size_out);
        version++;
    }

    deletestack(dfs_stack);

    gettimeofday(&tv2, NULL);
    printf("%f sec: Grail index creation\n",
           (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

    return grail;
}

// sinartisi pou epistrefei tin pithanotita na uparxei monopati ap ton source ston target
int isReachableGrailIndex(GrailIndex *index, uint32_t source_node, uint32_t target_node, SCC *scc) {

    uint32_t j = 0,
            scc_source, scc_target;

    scc_source = scc->id_belongs_to_component[source_node];
    scc_target = scc->id_belongs_to_component[target_node];

    if (scc_source == DEFAULT || scc_target == DEFAULT)
        return NO;
    if (scc_source == scc_target)
        return YES;

    for (j = 0; j < NUM_GRAIL; j++) {

        if ((index->hyper_index_out[scc_source].s_data->min_rank[j] > index->hyper_index_out[scc_target].s_data->min_rank[j]) ||
            (index->hyper_index_out[scc_source].s_data->rank[j] < index->hyper_index_out[scc_target].s_data->rank[j]))
            return NO;
    }
    return MAYBE;
}

// Free Grail index
void destroyGrailIndex(GrailIndex *index) {

    destroyBuffer(index->hyper_buffer_out);
    destroyNodeIndex(index->hyper_index_out, index->ind_size_out, 2);

    free(index);
}
