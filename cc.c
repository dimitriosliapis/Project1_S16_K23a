#include "cc.h"


void push(Stack *stack, uint32_t id) {

    sNode *new = malloc(sizeof(sNode));

    new->id = id;
    new->next = stack->last;

    stack->last = new;
}

uint32_t pop(Stack *stack) {

    sNode *rem = stack->last;
    uint32_t id;

    if (rem == NULL) return DEFAULT;

    id = rem->id;
    stack->last = rem->next;
    free(rem);
    return id;
}

int stackIsEmpty(Stack *stack) {
    if (stack == NULL) return 1;
    if (stack->last == NULL) return 1;
    return 0;
}

void deleteStack(Stack *stack, sNode *current) {

    if (stack == NULL) return;
    if (stack->last == NULL) {
        free(stack);
        return;
    }
    if (current->next == NULL) {
        free(current);
        return;
    }
    deleteStack(stack, current->next);
}

uint32_t createCCIndex(uint32_t *cc_index, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in,
                       uint32_t size_out) {

    ht_Node *explored = createHashtable(HT_BIG);
    Stack stack;
    uint32_t cur = 0;
    uint32_t v = 0;
    list_node *neighbors_in, *neighbors_out;
    uint32_t cc_counter = 0;
    ind *cur_ind = NULL;
    ptrdiff_t offset_in, offset_out;
    uint32_t i = 0;
    uint32_t size;

    if(size_in > size_out) size = size_in;
    else size = size_out;

    stack.last = NULL;

    cc_counter = 0;
    for (cur = 0; cur < size; cur++) {
        if((lookup(index_in, cur, size_in) == NOT_EXIST) && (lookup(index_out, cur, size_out) == NOT_EXIST)) continue;
        if (search(explored, cur, HT_BIG) == FOUND) continue; // visited
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);


            if (search(explored, v, HT_BIG) == NOT_FOUND) {
                cc_index[v] = cc_counter;
                insert(explored, v, HT_BIG);

                offset_in = getListHead(index_in, v);
                offset_out = getListHead(index_out, v);
                neighbors_in = buffer_in + offset_in;
                neighbors_out = buffer_out + offset_out;

                if(offset_in >= 0) {
                    i = 0;
                    while (i < N) {
                        if (neighbors_in->neighbor[i] == DEFAULT) break;
                        push(&stack, neighbors_in->neighbor[i]);
                        i++;
                        if (i == N && neighbors_in->nextListNode != -1) {
                            neighbors_in = buffer_in + neighbors_in->nextListNode;
                            i = 0;
                        }
                    }
                }
                if(offset_out >= 0) {
                    i = 0;
                    while (i < N) {
                        if (neighbors_out->neighbor[i] == DEFAULT) break;
                        push(&stack, neighbors_out->neighbor[i]);
                        i++;
                        if (i == N && neighbors_out->nextListNode != -1) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            i = 0;
                        }
                    }
                }
            }

        }
        cc_counter++;
    }

    return cc_counter;
}

int refreshUpdateIndex(uint32_t *cc_index, uint32_t **updateIndex, uint32_t *init_update_node_size, uint32_t *update_index_size, uint32_t N1, uint32_t N2) {

    uint32_t cc1 = cc_index[N1];
    uint32_t cc2 = cc_index[N2];
    uint32_t i = 0, realloc_size = 0, realloc_update_index_size = *update_index_size;
    uint32_t update_node_size = *init_update_node_size;
    uint32_t *temp = NULL;

    /*if(cc1 < *update_index_size && cc2 < *update_index_size) {*/
        if (cc1 != cc2) {
            //gia to N1
            if (updateIndex[cc1] == NULL) {
                updateIndex[cc1] = malloc(update_node_size * sizeof(uint32_t));
                temp = updateIndex[cc1];
                temp[0] = cc2;
                i = 1;
                while (i < update_node_size) {
                    temp[i] = DEFAULT;
                    i++;
                }
            } else {
                temp = updateIndex[cc1];
                i = 0;
                while (temp[i] != DEFAULT && i < update_node_size) i++;
                if (i < update_node_size) temp[i] = cc2;
                else if (i == update_node_size) {
                    realloc_size = 2 * update_node_size;
                    uint32_t *new = NULL;
                    new = realloc(updateIndex[cc1], realloc_size * sizeof(uint32_t));
                    new[i] = cc2;
                    for (i = update_node_size + 1; i < realloc_size; i++) new[i] = DEFAULT;
                    updateIndex[cc1] = new;
                    update_node_size = realloc_size;
                }
            }

            update_node_size = *init_update_node_size;
            //gia to N2
            if (updateIndex[cc2] == NULL) {
                updateIndex[cc2] = malloc(update_node_size * sizeof(uint32_t));
                temp = updateIndex[cc2];
                temp[0] = cc1;
                i = 1;
                while (i < update_node_size) {
                    temp[i] = DEFAULT;
                    i++;
                }
                return update_node_size;
            } else {
                i = 1;
                temp = updateIndex[cc2];
                while (temp[i] != DEFAULT && i < update_node_size) i++;
                if (i < update_node_size) {
                    temp[i] = cc1;
                    return update_node_size;
                }
                else if (i == update_node_size) {
                    realloc_size = 2 * update_node_size;
                    uint32_t *new2 = NULL;
                    new2 = realloc(updateIndex[cc2], realloc_size * sizeof(uint32_t));
                    new2[i] = cc1;
                    for (i = update_node_size + 1; i < realloc_size; i++) new2[i] = DEFAULT;
                    updateIndex[cc2] = new2;
                    update_node_size = realloc_size;
                    return update_node_size;
                }
            }
        }
    /*}*/
    /*else {
        //ama to index den xwraei
        while((realloc_update_index_size < cc1) || (realloc_update_index_size < cc2)) realloc_update_index_size = realloc_update_index_size * 2;
        updateIndex = realloc(updateIndex, realloc_update_index_size * sizeof(uint32_t *));
        for (i = *update_index_size + 1; i < realloc_update_index_size; i++) updateIndex[i] = NULL;
        *update_index_size = realloc_update_index_size;

        //gia to N1
        if (updateIndex[cc1] == NULL) {
            updateIndex[cc1] = malloc(update_node_size * sizeof(uint32_t));
            temp = updateIndex[cc1];
            temp[0] = cc2;
            i = 1;
            while (i < update_node_size) {
                temp[i] = DEFAULT;
                i++;
            }
        } else {
            temp = updateIndex[cc1];
            while (temp[i] != DEFAULT && i < update_node_size) i++;
            if (i < update_node_size) temp[i] = cc2;
            else if (i == update_node_size) {
                realloc_size = 2 * update_node_size;
                uint32_t *new = NULL;
                new = realloc(updateIndex[cc1], realloc_size * sizeof(uint32_t));
                new[i] = cc2;
                for (i = update_node_size + 1; i < realloc_size; i++) new[i] = DEFAULT;
                updateIndex[cc1] = new;
                update_node_size = realloc_size;
            }
        }

        //gia to N2
        if (updateIndex[cc2] == NULL) {
            updateIndex[cc2] = malloc(update_node_size * sizeof(uint32_t));
            temp = updateIndex[cc2];
            temp[0] = cc1;
            i = 1;
            while (i < update_node_size) {
                temp[i] = DEFAULT;
                i++;
            }
            return update_node_size;
        } else {
            i = 1;
            temp = updateIndex[cc2];
            while (temp[i] != DEFAULT && i < update_node_size) i++;
            if (i < update_node_size) {
                temp[i] = cc1;
                return update_node_size;
            }
            else if (i == update_node_size) {
                realloc_size = 2 * update_node_size;
                uint32_t *new2 = NULL;
                new2 = realloc(updateIndex[cc2], realloc_size * sizeof(uint32_t));
                new2[i] = cc1;
                for (i = update_node_size + 1; i < realloc_size; i++) new2[i] = DEFAULT;
                updateIndex[cc2] = new2;
                update_node_size = realloc_size;
                return update_node_size;
            }
        }
    }*/
}

int searchUpdateIndex(uint32_t *cc_index,uint32_t **updateIndex, uint32_t N1, uint32_t N2) {

    ht_Node *explored = createHashtable(HT_BIG);
    uint32_t cc1 = cc_index[N1];
    uint32_t cc2 = cc_index[N2];
    uint32_t v = 0;
    uint32_t *temp = NULL;
    uint32_t i = 0;
    Stack stack;
    stack.last = NULL;
    uint32_t update_node_size = 0;

    if(cc1 != cc2) {
        push(&stack, cc1);
        while(!stackIsEmpty(&stack)) {
            v = pop(&stack);
            if(v != cc2) {
                if (search(explored, v, HT_BIG) == NOT_FOUND) {
                    insert(explored, v, HT_BIG);

                    temp = updateIndex[v];
                    if(temp == NULL) continue;
                    i = 0;
                    update_node_size = sizeof(updateIndex[v])/sizeof(uint32_t);
                    while(temp[i] != DEFAULT && i < update_node_size) {
                        push(&stack, temp[i]);
                        i++;
                    }
                }
            }
            else return FOUND;
        }
        return NOT_FOUND;
    }
    else{
        return FOUND;
    }
}

uint32_t findCCMax(uint32_t *cc_index, uint32_t cc_index_size){

    uint32_t i = 0;
    uint32_t max = 0;

    for(i = 0; i < cc_index_size; i++){
        if(cc_index[i] > max) max = cc_index[i];
    }

    return max;
}


int updateCCIndex(uint32_t *cc_index, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in,
                  uint32_t size_out, uint32_t **updateIndex, uint32_t *cc_index_size, uint32_t update_index_size) {

    ht_Node *explored = createHashtable(HT_BIG);
    ht_Node *explored_new = createHashtable(HT_BIG);
    uint32_t v = 0;
    uint32_t v_new = 0;
    uint32_t i = 0, j = 0;
    Stack stack, stack_new;
    uint32_t parent_cc = 0;
    uint32_t new_size = *cc_index_size;
    uint32_t max = findCCMax(cc_index, *cc_index_size);
    ptrdiff_t offset_in, offset_out;
    list_node *neighbors_in, *neighbors_out;

    stack.last = NULL;
    stack_new.last = NULL;

    for(i = 0; i < *cc_index_size; i++ ) {

        parent_cc = cc_index[i];
////////////////////////////////////////////////////////////////////////
        if(parent_cc == DEFAULT){
            if((lookup(index_in, parent_cc, size_in) == NOT_EXIST) && (lookup(index_out, parent_cc, size_out) == NOT_EXIST)) continue;
            if (search(explored_new, parent_cc, HT_BIG) == FOUND) continue; // visited
            push(&stack_new, parent_cc);
            while (!stackIsEmpty(&stack_new)) {
                v_new = pop(&stack_new);


                if (search(explored_new, v_new, HT_BIG) == NOT_FOUND) {
                    cc_index[v_new] = i;
                    insert(explored_new, v_new, HT_BIG);
                    insert(explored, i, HT_BIG);//gia na min to tsekarei pali apo katw

                    offset_in = getListHead(index_in, v_new);
                    offset_out = getListHead(index_out, v_new);
                    neighbors_in = buffer_in + offset_in;
                    neighbors_out = buffer_out + offset_out;

                    if(offset_in >= 0) {
                        i = 0;
                        while (i < N) {
                            if (neighbors_in->neighbor[i] == DEFAULT) break;
                            push(&stack_new, neighbors_in->neighbor[i]);
                            i++;
                            if (i == N && neighbors_in->nextListNode != -1) {
                                neighbors_in = buffer_in + neighbors_in->nextListNode;
                                i = 0;
                            }
                        }
                    }
                    if(offset_out >= 0) {
                        i = 0;
                        while (i < N) {
                            if (neighbors_out->neighbor[i] == DEFAULT) break;
                            push(&stack_new, neighbors_out->neighbor[i]);
                            i++;
                            if (i == N && neighbors_out->nextListNode != -1) {
                                neighbors_out = buffer_out + neighbors_out->nextListNode;
                                i = 0;
                            }
                        }
                    }
                }

            }
        }
/////////////////////////////////////////////////////////////////////////
        else {
            if (search(explored, parent_cc, HT_BIG) == FOUND) continue;

            push(&stack, parent_cc);

            while (!stackIsEmpty(&stack)) {

                v = pop(&stack);
                cc_index[v] = parent_cc;

                insert(explored, v, HT_BIG);

                for (j = 0; j < (sizeof(updateIndex[v]) / sizeof(uint32_t)); j++) {
                    push(&stack, updateIndex[v][j]);
                }

            }
        }
    }
}