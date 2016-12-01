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
            prev->next = cur->next;
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

SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, uint32_t size_out, ind *index_in, list_node *buffer_in, uint32_t size_in, uint32_t num_nodes/*, ht_Node *explored*/){

    Stack stack;
    SCC *scc = tarjan(index_out, buffer_out, size_out, num_nodes);
    uint32_t i = 0, j = 0, k = 0;
    ptrdiff_t available_in = 0;
    ptrdiff_t available_out = 0;
    ptrdiff_t offset_in;
    ptrdiff_t offset_out;
    list_node *neighbors_in, *neighbors_out;
    uint32_t neigh_scc = 0;


    scc->buf_size_in = 2*scc->components_count;
    scc->buf_size_out = 2*scc->components_count;

    scc->hyper_buffer_in = createBuffer(scc->buf_size_in);
    scc->hyper_index_in = createNodeIndex(scc->components_count);
    scc->hyper_buffer_out = createBuffer(scc->buf_size_out);
    scc->hyper_index_out = createNodeIndex(scc->components_count);


    for(i = 0; i < scc->components_count; i++){
        insertNode(&scc->hyper_index_out, i, &scc->hyper_buffer_out, &scc->components_count, &scc->buf_size_out, &available_out);
        insertNode(&scc->hyper_index_in, i, &scc->hyper_buffer_in, &scc->components_count, &scc->buf_size_in, &available_in);
    }

    for(i = 0; i < scc->components_count; i++){

        for(j = 0; j < scc->components[i].included_nodes_count; j++){

            offset_out = getListHead(index_out, scc->components[i].included_node_ids[j]);
            neighbors_out = buffer_out + offset_out;

            k = 0;
            while (k < N) {
                if (neighbors_out->neighbor[k] == DEFAULT) break;

                neigh_scc = scc->id_belongs_to_component[neighbors_out->neighbor[k]];

                if(neigh_scc != i){
                    addEdge(&scc->hyper_index_out, i, neigh_scc, &scc->hyper_buffer_out, &scc->buf_size_out, &available_out);
                }

                k++;
                if (k == N) {
                    if (neighbors_out->nextListNode > 0) {
                        neighbors_out = buffer_out + neighbors_out->nextListNode;  // an uparxei sunexizei se auton
                        k = 0;
                    }
                }
            }



            offset_in = getListHead(index_in, scc->components[i].included_node_ids[j]);
            neighbors_in = buffer_in + offset_in;

            k = 0;
            while (k < N) {
                if (neighbors_in->neighbor[k] == DEFAULT) break;

                neigh_scc = scc->id_belongs_to_component[neighbors_in->neighbor[k]];

                if(neigh_scc != i){
                    addEdge(&scc->hyper_index_in, i, neigh_scc, &scc->hyper_buffer_in, &scc->buf_size_in, &available_in);
                }

                k++;
                if (k == N) {
                    if (neighbors_in->nextListNode > 0) {
                        neighbors_in = buffer_in + neighbors_in->nextListNode;  // an uparxei sunexizei se auton
                        k = 0;
                    }
                }
            }
        }
    }


    return scc;
}

SCC* tarjan(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes) {

    Stack stack;
    Stack parent_stack;
    ht_Node *explored = createHashtable(HT_BIG);
    ht_Node *explored_stack = createHashtable(HT_BIG);
    list_node *neighbors_out;
    ptrdiff_t offset_out;
    uint32_t size = size_out, index = 0;
    uint32_t i = 0, v = 0, k = 0, a = 0, check = 0;
    uint32_t scc_counter = 0;

    SCC *scc = malloc(sizeof(SCC));

    scc->components = malloc(COMPONENT_SIZE*sizeof(Component));
    //for(k = 0 ; k < COMPONENT_SIZE ; k++) scc->components[k] = NULL;
    scc->components_count = DEFAULT;

    scc->id_belongs_to_component = malloc(num_nodes*sizeof(uint32_t));
    for(i = 0; i < num_nodes; i++){
        scc->id_belongs_to_component[i] = DEFAULT;
    }


    stack.last = NULL;
    parent_stack.last = NULL;

    for(i = 0 ; i < size ; i++) {
        if(lookup(index_out, i, size_out) == NOT_EXIST) continue;
        if(search(explored, i, HT_BIG) == FOUND) continue;

        push(&stack, i);
        index_out[i].lowlink = index;
        index_out[i].index = index;
        index++;

        while(!stackIsEmpty(&stack)) {
            if(check == 0) {
                v = peek(&stack);
                //if(v == DEFAULT) return;
            }
            else {
                v = nextfromStack(&parent_stack, v);
                //if(v == DEFAULT) return;
            }
            check = 0;

            if(onStack(&parent_stack, v)) {

                offset_out = getListHead(index_out, v);
                neighbors_out = buffer_out + offset_out;

                k = 0;
                while (k < N) {
                    if(neighbors_out->neighbor[k] == DEFAULT) break;
                    if(index_out[v].lowlink > index_out[neighbors_out->neighbor[k]].lowlink) {
                        index_out[v].lowlink = index_out[neighbors_out->neighbor[k]].lowlink;
                    }
                    else if(onStack(&stack, k)) {
                        if(index_out[v].lowlink > index_out[neighbors_out->neighbor[k]].index) index_out[v].lowlink = index_out[neighbors_out->neighbor[k]].index;
                    }
                    k++;
                    if(k == N && neighbors_out->nextListNode != -1) {
                        neighbors_out = buffer_out + neighbors_out->nextListNode;
                        k = 0;
                    }
                }
            }

            if(search(explored, v, HT_BIG) == NOT_FOUND || onStack(&parent_stack, v)) {
                if(search(explored, v, HT_BIG) == NOT_FOUND) insert(explored, v, HT_BIG);

                offset_out = getListHead(index_out, v);
                neighbors_out = buffer_out + offset_out;

                if(offset_out >= 0 && !onStack(&parent_stack, v)) {
                    push(&parent_stack, v);
                    k = 0;

                    while(k < N) {
                        if(neighbors_out->neighbor[k] == DEFAULT) break;
                        if(search(explored, neighbors_out->neighbor[k], HT_BIG) == NOT_FOUND) {
                            push(&stack, neighbors_out->neighbor[k]);
                            index_out[neighbors_out->neighbor[k]].lowlink = index;
                            index_out[neighbors_out->neighbor[k]].index = index;
                            index++;
                        }
                        k++;
                        if(k == N && neighbors_out->nextListNode != -1) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            k = 0;
                        }
                    }
                }
                else if(index_out[v].lowlink == index_out[v].index) {
                    scc->components[scc_counter].component_id = scc_counter;
                    scc->components[scc_counter].included_node_ids = malloc(NODE_IDS_SIZE*sizeof(uint32_t));
                    for(a = 0 ; a < NODE_IDS_SIZE ; a++) scc->components[scc_counter].included_node_ids[a] = DEFAULT;
                    k = DEFAULT;
                    a = 0;
                    while (k != v) {
                        k = pop(&stack);
                        if(k == DEFAULT) break;
                        if(search(explored_stack, k, HT_BIG) == FOUND) continue;
                        insert(explored_stack, k, HT_BIG);
                        if(onStack(&parent_stack, k)) removefromStack(&parent_stack, k);

                        scc->components[scc_counter].included_node_ids[a] = k;
                        scc->components[scc_counter].included_nodes_count++;
                        scc->id_belongs_to_component[k] = scc_counter;
                        a++;
                        printf("ta scc %d exei ta %d ids\n", scc_counter, k);
                    }

                    scc_counter++;
                }
                else {
                    pop(&parent_stack);
                    v = peek(&parent_stack);
                    //if(v == DEFAULT) return;
                    check = 1;
                }
            }
        }
    }

    scc->components_count = scc_counter;

    delete(explored, HT_BIG);
    delete(explored_stack, HT_BIG);



    return scc;

}