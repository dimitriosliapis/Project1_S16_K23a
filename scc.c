#include "scc.h"

uint32_t peek(Stack *stack) {

    if(stack == NULL) return DEFAULT;
    if(stack->last == NULL) return DEFAULT;

    return stack->last->id;

}

void removefromStack(Stack *stack, uint32_t id) {

    if(stack == NULL) return;
    if(stack->last == NULL) return;

    sNode *cur = NULL;
    sNode *prev = NULL;

    if(stack->last->id == id){
        pop(stack);
        return;
    }



    cur = stack->last;
    prev = cur;
    while (cur!=NULL) {
        if(cur->id == id) {
            prev->next = cur;
            free(prev);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

uint32_t nextfromStack(Stack *stack, uint32_t id) {

    if(stack == NULL) return DEFAULT;
    if(stack->last == NULL) return DEFAULT;

    sNode *cur = NULL;
    sNode *prev = NULL;

    cur = stack->last;
    prev = cur;

    while(cur != NULL) {
        if(cur->id == id) return prev->id;
        prev = cur;
        cur = cur->next;
    }

    return DEFAULT;
}

SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes){


    SCC *scc = tarjan(index_out, buffer_out, size_out, num_nodes);

    return scc;
}

SCC* tarjan(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes) {

    Stack scc_stack;
    Stack dfs_stack;
    ht_Node *explored = createHashtable(HT_BIG);
    ht_Node *explored_twice = createHashtable(HT_BIG);
    ht_Node *explored_scc = createHashtable(HT_BIG);
    list_node *neighbors_out;
    ptrdiff_t offset_out;
    uint32_t size = size_out, index = 0, realloc_node_size;
    uint32_t i = 0, v = 0, k = 0, a = 0, w = 0, caller = DEFAULT;
    uint32_t scc_counter = 0;
    int all_child_in_scc = 0;

    SCC *scc = malloc(sizeof(SCC));

    scc->components = malloc(COMPONENT_SIZE*sizeof(Component));
    //for(k = 0 ; k < COMPONENT_SIZE ; k++) scc->components[k] = NULL;
    scc->components_count = 0;

    scc->id_belongs_to_component = malloc(num_nodes*sizeof(uint32_t));
    for(i = 0; i < num_nodes; i++){
        scc->id_belongs_to_component[i] = DEFAULT;
    }


    scc_stack.last = NULL;
    dfs_stack.last = NULL;


    for(i = 0 ; i < size ; i++) {
        if(lookup(index_out, i, size_out) == NOT_EXIST) continue;
        if(search(explored_scc, i, HT_BIG) == FOUND) continue;

        caller = DEFAULT;

        index = 1;

        push(&dfs_stack, i);
        push(&scc_stack, i);

        while(!stackIsEmpty(&scc_stack)){

            v = peek(&dfs_stack);
            if(search(explored, v, HT_BIG) == NOT_FOUND) {
                index_out[v].index = index;
                index_out[v].lowlink = index;
                index++;
                insert(explored, v, HT_BIG);
            }


            offset_out = getListHead(index_out, v);
            neighbors_out = buffer_out + offset_out;

            if(offset_out >= 0 && neighbors_out->neighbor[0] != DEFAULT && all_child_in_scc == 0){   //if v has children

                if(search(explored_twice, v, HT_BIG) == FOUND) {
                    pop(&dfs_stack);
                }
                k = 0;
                while (k < N) {
                    w = neighbors_out->neighbor[k];
                    if(w == DEFAULT) break;

                    if(search(explored_scc, w, HT_BIG) == FOUND){
                        k++;
                        all_child_in_scc = 1;
                        continue;
                    }
                    all_child_in_scc = 0;
                    if(search(explored, w, HT_BIG) == NOT_FOUND){
                        index_out[w].index = index;
                        index_out[w].lowlink = index;
                        index++;
                        push(&scc_stack, w);
                        push(&dfs_stack, w);
                        insert(explored, w, HT_BIG);

                    }
                    else{
                        if(search(explored_twice, w, HT_BIG) == NOT_FOUND){

                            if (index_out[v].lowlink > index_out[w].lowlink)
                                index_out[v].lowlink = index_out[w].lowlink;

                            push(&dfs_stack, w);
                            caller = v;
                            insert(explored_twice, w, HT_BIG);
                        }
                        else{

                            if(index_out[v].lowlink > index_out[w].index) index_out[v].lowlink = index_out[w].index;

                            scc->components[scc_counter].component_id = scc_counter;
                            scc->components[scc_counter].included_nodes_count = 0;
                            scc->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                            scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
                            for(a = 0 ; a < scc->components[scc_counter].node_array_size ; a++) scc->components[scc_counter].included_node_ids[a] = DEFAULT;
                            a = 0;


                            v = pop(&dfs_stack);
                            while(index_out[v].index != index_out[v].lowlink) v = pop(&dfs_stack);
                            while(1){
                                w = pop(&scc_stack);
                                if(w == DEFAULT) break;
                                insert(explored_scc, w, HT_BIG);

                                scc->components[scc_counter].included_node_ids[a] = w;

                                if(a == (scc->components[scc_counter].node_array_size-1)) {
                                    realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                                    scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
                                    for(a = scc->components[scc_counter].node_array_size ; a < realloc_node_size ; a++) scc->components[scc_counter].included_node_ids[a] = DEFAULT;
                                    scc->components[scc_counter].node_array_size = realloc_node_size;
                                }
                                scc->components[scc_counter].included_nodes_count++;
                                scc->id_belongs_to_component[w] = scc_counter;

                                a++;
                                printf("ta scc %d exei ta %d ids\n", scc_counter, w);
                                if(w == v) break;
                            }
                            scc_counter++;
                        }
                    }
                    k++;
                    if(k >= N && neighbors_out->nextListNode != -1) {
                        neighbors_out = buffer_out + neighbors_out->nextListNode;
                        k = 0;
                    }
                }

            }
            else{
                w = pop(&scc_stack);
                pop(&dfs_stack);

                insert(explored, w, HT_BIG);
                insert(explored_scc, w, HT_BIG);

                scc->components[scc_counter].component_id = scc_counter;
                scc->components[scc_counter].included_nodes_count = 0;
                scc->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
                for(a = 0 ; a < scc->components[scc_counter].node_array_size ; a++) scc->components[scc_counter].included_node_ids[a] = DEFAULT;
                a = 0;

                scc->components[scc_counter].included_node_ids[a] = w;

                if(a == (scc->components[scc_counter].node_array_size-1)) {
                    realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                    scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
                    for(a = scc->components[scc_counter].node_array_size ; a < realloc_node_size ; a++) scc->components[scc_counter].included_node_ids[a] = DEFAULT;
                    scc->components[scc_counter].node_array_size = realloc_node_size;
                }
                scc->components[scc_counter].included_nodes_count++;
                scc->id_belongs_to_component[w] = scc_counter;

                a++;
                printf("ta scc %d exei ta %d ids\n", scc_counter, w);
                scc_counter++;
            }
        }
    }
    scc->components_count = scc_counter;
    delete(explored, HT_BIG);
    delete(explored_scc, HT_BIG);



    return scc;

}