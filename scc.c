#include "scc.h"

uint32_t peek(Stack *stack) {

    return stack->last->id;

}

void removefromStack(Stack *stack, uint32_t id) {

    if(stack == NULL) return;
    if(stack->last == NULL) return;

    sNode *cur = NULL;
    sNode *prev;

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

    if(stack == NULL) return 0;
    if(stack->last == NULL) return 0;

    sNode *cur = NULL;
    sNode *prev = NULL;

    cur = stack->last;
    prev = cur;

    while(cur != NULL) {
        if(cur->id == id) return prev->id;
        prev = cur;
        cur = cur->next;
    }

    return 0;
}

void tarjan(ind *index_out, list_node *buffer_out, uint32_t size_out) {

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
            if(check == 0) v = peek(&stack);
            else v = nextfromStack(&parent_stack, v);
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
                        a++;
                        printf("ta scc %d exei ta %d ids\n", scc_counter, k);
                    }

                    scc_counter++;
                }
                else {
                    pop(&parent_stack);
                    v = peek(&parent_stack);
                    check = 1;
                }
            }
        }
    }
    delete(explored, HT_BIG);
    delete(explored_stack, HT_BIG);
}