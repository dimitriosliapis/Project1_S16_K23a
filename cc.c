#include "cc.h"

// Create stack
Stack_t *createStack() {

    int i = 0;
    Stack_t *stack = NULL;
    stack = malloc(sizeof(Stack_t));

    if (stack == NULL)
        return NULL;
    else {
        stack->size = STACK_ARRAY_SIZE;
        stack->stack_array = malloc(STACK_ARRAY_SIZE * sizeof(uint32_t));
        for (i = 0; i < STACK_ARRAY_SIZE; i++) stack->stack_array[i] = DEFAULT;
        stack->last = -1;
        stack->count = 0;
        return stack;
    }
}

// Check if stack is empty
int stackisempty(Stack_t *stack) {

    if (stack->count == 0)
        return 1;
    else
        return 0;
}

// Push element in stack
void pushinstack(Stack_t *stack, uint32_t id) {

    if (stack->count == stack->size) {
        stack->stack_array = realloc(stack->stack_array, 2 * stack->size * sizeof(uint32_t));
        stack->size = 2 * stack->size;
        int i = 0;
        for (i = stack->size / 2; i < stack->size; i++)
            stack->stack_array[i] = DEFAULT;
    }

    stack->last = (stack->last + 1) % stack->size;
    stack->stack_array[stack->last] = id;
    stack->count++;
}

// Pop last element pushed in stack
uint32_t popfromstack(Stack_t *stack) {

    uint32_t id = DEFAULT;
    if (stack->count == 0)
        return DEFAULT;

    id = stack->stack_array[stack->last];
    stack->stack_array[stack->last] = DEFAULT;
    stack->last = (stack->last - 1) % stack->size;
    stack->count--;
    return id;
}

// Peek last element pushed in stack
uint32_t peekfromstack(Stack_t *stack) {

    uint32_t id = DEFAULT;

    if (stack->stack_array[stack->last] == DEFAULT)
        return DEFAULT;

    id = stack->stack_array[stack->last];
    return id;
}

// Free stack
void deletestack(Stack_t *stack) {

    free(stack->stack_array);
    free(stack);
}

// Create CC index
CC *createCCIndex(uint32_t cc_size, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out,
                  uint32_t size_in, uint32_t size_out, uint32_t version) {


    Stack_t *stack = createStack();
    uint32_t cur = 0, v = 0, i = 0, j = 0, cc_counter = 0;
    list_node *neighbors_in, *neighbors_out;
    ptrdiff_t offset_in, offset_out;
    int in, out;

    // memory allocation and initialisation
    CC *cc = malloc(sizeof(CC));
    CC_ind *cc_index = malloc(sizeof(CC_ind) * cc_size);

    for (i = 0; i < cc_size; i++) {
        cc_index[i].id = DEFAULT;
        for (j = 0; j <= THREAD_POOL_SIZE; j++)
            cc_index[i].visited[j] = DEFAULT;
    }

    cc->cc_index = cc_index;
    cc->cc_size = cc_size;

    cc_counter = 0;

    for (cur = 0; cur < cc_size; cur++) {
        in = lookup(index_in, cur, size_in);
        out = lookup(index_out, cur, size_out);
        if ((in == NOT_EXIST) && (out == NOT_EXIST))
            continue;
        if ((in == ALR_EXISTS) && (index_in[cur].visited[0] == version))
            continue;
        if ((out == ALR_EXISTS) && (index_out[cur].visited[0] == version))
            continue;

        // push node and neighbors in stack and do the same for all their neighbors etc.
        // they all belond to the same cc
        pushinstack(stack, cur);

        while (!stackisempty(stack)) {

            v = popfromstack(stack);

            in = lookup(index_in, v, size_in);
            out = lookup(index_out, v, size_out);

            if (((in == ALR_EXISTS) && (index_in[v].visited[0] != version)) ||
                ((out == ALR_EXISTS) && (index_out[v].visited[0] != version))) {

                cc_index[v].id = cc_counter;

                if (in == ALR_EXISTS)
                    index_in[v].visited[0] = version;
                if (out == ALR_EXISTS)
                    index_out[v].visited[0] = version;

                offset_in = getListHead(index_in, v);
                offset_out = getListHead(index_out, v);
                neighbors_in = buffer_in + offset_in;
                neighbors_out = buffer_out + offset_out;

                if (offset_in >= 0) {

                    i = 0;
                    while (i < N) {

                        if (neighbors_in->neighbor[i] == DEFAULT)
                            break;

                        pushinstack(stack, neighbors_in->neighbor[i]);

                        i++;
                        if ((i == N) && (neighbors_in->nextListNode != -1)) {
                            neighbors_in = buffer_in + neighbors_in->nextListNode;
                            i = 0;
                        }
                    }
                }

                if (offset_out >= 0) {

                    i = 0;
                    while (i < N) {

                        if (neighbors_out->neighbor[i] == DEFAULT)
                            break;

                        pushinstack(stack, neighbors_out->neighbor[i]);

                        i++;
                        if ((i == N) && (neighbors_out->nextListNode != -1)) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            i = 0;
                        }
                    }
                }
            }
        }
        cc_counter++;   // next cc

    }

    cc->cc_max = cc_counter;    // when finish keeps track of the last cc created
    deletestack(stack);

    return cc;
}

// Initialise Update index
void initUpdateIndex(CC *cc) {

    u_node *update_index = malloc(cc->u_size * sizeof(u_node));

    uint32_t a = 0;
    uint32_t j = 0;

    while (a < cc->u_size) {
        update_index[a].cc_array = NULL;
        update_index[a].new_nodes = NULL;
        update_index[a].size = 0;
        update_index[a].n_size = 0;
        update_index[a].state = 'o';//old

        for (j = 0; j <= THREAD_POOL_SIZE; j++)
            update_index[a].visited[j] = DEFAULT;

        a++;
    }

    cc->updateIndex = update_index;
}

// Refresh Update index each time an edge is added
void refreshUpdateIndex(CC *cc, uint32_t N1, uint32_t N2) {

    uint32_t cc1, cc2, new_cc = 0, realloc_size = 0, realloc_update_index_size,
            i = 0, k = 0, l = 0, j = 0,
            *temp = NULL;
    uint32_t found = 0;

    // if either N1 or N2 does not exist then cc = DEFAULT
    // in this case we check if they are larger than cc index
    if (N1 >= cc->cc_size)
        cc1 = DEFAULT;
    else
        cc1 = cc->cc_index[N1].id;

    if (N2 >= cc->cc_size)
        cc2 = DEFAULT;
    else
        cc2 = cc->cc_index[N2].id;

    // if they don't exist then they are already DEFAULT
    // if they are not DEFAULT and are equal they are already connected

    if ((cc1 != DEFAULT) && (cc1 == cc2))
        return;

    // if we are about to create new cc
    if ((cc1 == DEFAULT) && (cc2 == DEFAULT)) {

        i = cc->cc_max;

        // if they can fit in the update index
        if (i < cc->u_size) {

            l = i;

            // search for the first available position in the update index
            while ((cc->updateIndex[l].state != 'e') && (l < cc->u_size))
                l++;    // ( e - > empty )

            // search if they are already in the update index
            while (l < cc->u_size) {

                if (cc->updateIndex[l].new_nodes != NULL) {

                    for (k = 0; k < cc->updateIndex[l].n_size; k++) {

                        if (cc->updateIndex[l].new_nodes[k] == DEFAULT)
                            break;
                        if (cc->updateIndex[l].new_nodes[k] == N1) {
                            cc1 = i;
                            if(found == N2) {
                                found+= N1; // count if found
                                break;
                            }
                        }
                        if (cc->updateIndex[l].new_nodes[k] == N2) {
                            cc2 = i;
                            if(found == N1) {
                                found+=N2; // count if found
                                break;
                            }
                        }
                    }
                    if(found == N1 + N2) break;
                }
                l++;
            }

            // if we haven't found neither create new cc
            if (!found) {

                if (l < cc->u_size) {
                    new_cc = l;

                } else {    // realloc if update index is full
                    new_cc = cc->u_size;
                    realloc_update_index_size = 2 * cc->u_size;
                    cc->updateIndex = realloc(cc->updateIndex, realloc_update_index_size * sizeof(u_node));

                    for (i = (new_cc + 1); i < realloc_update_index_size; i++) {
                        cc->updateIndex[i].cc_array = NULL;
                        cc->updateIndex[i].size = 0;
                        cc->updateIndex[i].state = 'e';
                        cc->updateIndex[i].new_nodes = NULL;
                        cc->updateIndex[i].n_size = 0;
                        for (j = 0; j < THREAD_POOL_SIZE; j++) cc->updateIndex[i].visited[j] = DEFAULT;

                    }
                    cc->u_size = realloc_update_index_size;

                }

                //  add nodes to the new cc
                cc->updateIndex[new_cc].state = 'n'; // ( n - > new )
                cc->updateIndex[new_cc].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[new_cc].new_nodes[0] = N1;
                cc->updateIndex[new_cc].new_nodes[1] = N2;
                cc->updateIndex[new_cc].new_nodes[3] = DEFAULT;
                for (j = 0; j < THREAD_POOL_SIZE; j++)
                    cc->updateIndex[new_cc].visited[j] = DEFAULT;

                return;

            } else
                if (cc1 == cc2) return;     // return if they are already in the same cc

        } else {

            realloc_size = 2 * cc->u_size;
            cc->updateIndex = realloc(cc->updateIndex, realloc_size * sizeof(u_node));
            new_cc = cc->u_size;
            cc->updateIndex[new_cc].state = 'n'; // ( n - > new )
            cc->updateIndex[new_cc].n_size = INIT_NEWNODE_SIZE;
            cc->updateIndex[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
            cc->updateIndex[new_cc].new_nodes[0] = N1;
            cc->updateIndex[new_cc].new_nodes[1] = N2;
            cc->updateIndex[new_cc].new_nodes[3] = DEFAULT;
            for (j = 0; j < THREAD_POOL_SIZE; j++)
                cc->updateIndex[new_cc].visited[j] = DEFAULT;

            for (l = new_cc + 1; l < realloc_size; l++) {
                cc->updateIndex[l].new_nodes = NULL;
                cc->updateIndex[l].state = 'e';
                cc->updateIndex[l].n_size = 0;
                cc->updateIndex[l].cc_array = NULL;
                cc->updateIndex[l].size = 0;
                for (j = 0; j < THREAD_POOL_SIZE; j++)
                    cc->updateIndex[l].visited[j] = DEFAULT;
            }
            cc->u_size = realloc_size;
            return;

        }
    }

    // if N1 or N2 doesn't have an cc then add them
    if (cc1 == DEFAULT) {

        found = 0;
        i = cc->cc_max;

        if (i < cc->u_size) {

            l = i;
            while ((cc->updateIndex[l].state != 'e') && (l < cc->u_size))
                l++;

            while (l < cc->u_size) {
                if (cc->updateIndex[l].new_nodes != NULL) {
                    for (k = 0; k < cc->updateIndex[l].n_size; k++) {
                        if (cc->updateIndex[l].new_nodes[k] == DEFAULT)
                            break;
                        if (cc->updateIndex[l].new_nodes[k] == N1) {
                            cc1 = l;
                            found = 1;
                            break;
                        }
                    }
                    if(found) break;
                }
                l++;

            }
        }

        // if it is not found in the update index
        if (!found) {

            // add N1 to update index for the cc2
            if (cc->updateIndex[cc2].new_nodes == NULL) {
                cc->updateIndex[cc2].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[cc2].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[cc2].new_nodes[0] = N1;
                cc->updateIndex[cc2].new_nodes[1] = DEFAULT;

            } else {

                k = 0;

                while ((cc->updateIndex[cc2].new_nodes[k] != DEFAULT) && (k < cc->updateIndex[cc2].n_size)) k++;

                if (k == cc->updateIndex[cc2].n_size) {
                    realloc_size = 2 * cc->updateIndex[cc2].n_size;
                    cc->updateIndex[cc2].new_nodes = realloc(cc->updateIndex[cc2].new_nodes, realloc_size * sizeof(uint32_t));

                    cc->updateIndex[cc2].new_nodes[k + 1] = DEFAULT;
                }
                cc->updateIndex[cc2].new_nodes[k] = N1;
            }
            return;

        }

    } else if (cc2 == DEFAULT) {

        found = 0;

        i = cc->cc_max;

        if (i < cc->u_size) {

            l = i;

            while ((cc->updateIndex[l].state != 'e') && (l < cc->u_size)) l++;

            while (l < cc->u_size) {
                if (cc->updateIndex[l].new_nodes != NULL) {
                    for (k = 0; k < cc->updateIndex[l].n_size; k++) {
                        if (cc->updateIndex[l].new_nodes[k] == DEFAULT) break;
                        if (cc->updateIndex[l].new_nodes[k] == N2) {
                            cc2 = l;
                            found = 1;
                            break;
                        }
                    }
                    if(found) break;
                }
                l++;
            }
        }

        if (!found) {
            if (cc->updateIndex[cc1].new_nodes == NULL) {
                cc->updateIndex[cc1].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[cc1].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[cc1].new_nodes[0] = N2;
                cc->updateIndex[cc1].new_nodes[1] = DEFAULT;
            } else {

                k = 0;

                while ((cc->updateIndex[cc1].new_nodes[k] != DEFAULT) && (k < cc->updateIndex[cc1].n_size)) k++;

                if (k == cc->updateIndex[cc1].n_size) {
                    realloc_size = 2 * cc->updateIndex[cc1].n_size;
                    cc->updateIndex[cc1].new_nodes = realloc(cc->updateIndex[cc1].new_nodes, realloc_size * sizeof(uint32_t));

                    cc->updateIndex[cc1].new_nodes[k + 1] = DEFAULT;
                }
                cc->updateIndex[cc1].new_nodes[k] = N2;
            }
            return;
        }
    }

    // if both exist in the cc index then connect them in the update index

    j = 0;
    uint32_t cur_cc = cc1;

    while (j < 2) {

        // if it isn't connected with another cc
        if (cc->updateIndex[cur_cc].cc_array == NULL) {
            cc->updateIndex[cur_cc].cc_array = malloc(INIT_UNODE_SIZE * sizeof(uint32_t));

            if (j == 0)     // check for the cc we are reffering to
                cc->updateIndex[cur_cc].cc_array[0] = cc2;
            else
                cc->updateIndex[cur_cc].cc_array[0] = cc1;

            cc->updateIndex[cur_cc].size = INIT_UNODE_SIZE;
            cc->updateIndex[cur_cc].cc_array[1] = DEFAULT;

        } else {
            i = 0;

            while ((cc->updateIndex[cur_cc].cc_array[i] != DEFAULT) && (i < cc->updateIndex[cur_cc].size))
                i++;

            if (i < cc->updateIndex[cur_cc].size) {
                if (j == 0)
                    cc->updateIndex[cur_cc].cc_array[i] = cc2;
                else
                    cc->updateIndex[cur_cc].cc_array[i] = cc1;

                if(i < cc->updateIndex[cur_cc].size - 1)
                    cc->updateIndex[cur_cc].cc_array[i + 1] = DEFAULT;

            } else if (i == cc->updateIndex[cur_cc].size) {
                realloc_size = 2 * cc->updateIndex[cur_cc].size;
                cc->updateIndex[cur_cc].cc_array = realloc(cc->updateIndex[cur_cc].cc_array, realloc_size * sizeof(uint32_t));

                if (j == 0)
                    cc->updateIndex[cur_cc].cc_array[i] = cc2;
                else
                    cc->updateIndex[cur_cc].cc_array[i] = cc1;

                cc->updateIndex[cur_cc].cc_array[i + 1] = DEFAULT;

                cc->updateIndex[cur_cc].size = realloc_size;
            }
        }
        j++;
        cur_cc = cc2;

    }
}

// Search Update index
int searchUpdateIndex(CC cc, uint32_t N1, uint32_t N2, uint32_t version, int thread_id) {

    uint32_t cc1 = 0;
    uint32_t cc2 = 0;
    uint32_t flag = 0;
    uint32_t v = 0;
    uint32_t i = 0, j = 0;
    Stack_t *stack = createStack();

    if (N1 >= cc.cc_size) cc1 = DEFAULT;
    else cc1 = cc.cc_index[N1].id;
    if (N2 >= cc.cc_size) cc2 = DEFAULT;
    else cc2 = cc.cc_index[N2].id;

    // if some node doesn't exist in the cc index search only the update index
    if (cc1 == DEFAULT || cc2 == DEFAULT) {
        i = 0;

        while (i < cc.u_size) {
            if (i == cc.u_size || cc.updateIndex[i].state == 'e') {
                deletestack(stack);
                return NOT_FOUND;
            }
            if (cc.updateIndex[i].new_nodes != NULL) {
                for (j = 0; j < cc.updateIndex[i].n_size; j++) {
                    if (cc.updateIndex[i].new_nodes[j] == DEFAULT) break;
                    else if (cc.updateIndex[i].new_nodes[j] == N1) {
                        cc1 = i;
                        flag++;
                        break;
                    } else if (cc.updateIndex[i].new_nodes[j] == N2) {
                        cc2 = i;
                        flag++;
                        break;
                    }
                }
                if(flag == 2) break;
            }
            i++;
        }
    }

    // if we haven't found at least one then they are not connected
    if (cc1 == DEFAULT || cc2 == DEFAULT) {
        deletestack(stack);
        return NOT_FOUND;
    }

    //search the update index to see if the 2 cc are connected
    if (cc1 != cc2) {

        pushinstack(stack, cc1);

        while (!stackisempty(stack)) {

            v = popfromstack(stack);

            if (v != cc2) {

                if (cc.updateIndex[v].visited[thread_id] != version) {

                    cc.updateIndex[v].visited[thread_id] = version;

                    if (cc.updateIndex[v].cc_array == NULL)
                        continue;

                    i = 0;

                    while ((cc.updateIndex[v].cc_array[i] != DEFAULT) && (i < cc.updateIndex[v].size)) {
                        pushinstack(stack, cc.updateIndex[v].cc_array[i]);
                        i++;
                    }
                }

            } else {
                deletestack(stack);
                return FOUND;
            }
        }
        deletestack(stack);
        return NOT_FOUND;

    } else {
        deletestack(stack);
        return FOUND;   // if cc1 = cc2
    }
}

// Update CC index
uint32_t updateCCIndex(CC *cc, uint32_t version, uint32_t new_size, int thread_id) {

    uint32_t v = 0, w = 0, i = 0, j = 0, k,
            parent_cc = 0, old_cc = 0;

    Stack_t *stack = createStack();
    uint32_t *new_cc = malloc(new_size * sizeof(uint32_t));

    // initialize array to change the cc index
    for (i = 0; i < new_size; i++) new_cc[i] = DEFAULT;

    // if the new cc doesn't fit realloc cc index
    if (new_size > cc->cc_size) {
        cc->cc_index = realloc(cc->cc_index, new_size * sizeof(uint32_t));
        for (j = cc->cc_size; j < new_size; j++)
            cc->cc_index[j].id = DEFAULT;
    }

    // insert new nodes to the cc index
    for (i = 0; i < cc->u_size; i++) {
        if (cc->updateIndex[i].new_nodes != NULL) {
            for (k = 0; k < cc->updateIndex[i].n_size; k++) {
                if (cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                cc->cc_index[cc->updateIndex[i].new_nodes[k]].id = i;
            }
        }
    }

    // reenumerate the cc (begin from 0)
    parent_cc = 0;

    // find for every cc the new id
    for (i = 0; i < cc->u_size; i++) {

        if (cc->updateIndex[i].state == 'e') break;

        if (cc->updateIndex[i].visited[thread_id] == version) continue;

        pushinstack(stack, i);
        new_cc[i] = parent_cc;

        cc->updateIndex[i].visited[thread_id] = version;

        while (!stackisempty(stack)) {
            v = popfromstack(stack);

            if (cc->updateIndex[v].cc_array != NULL) {
                for (k = 0 ; k < cc->updateIndex[v].size ; k++) {
                    w = cc->updateIndex[v].cc_array[k];
                    if (w == DEFAULT) break;
                    if (new_cc[w] != DEFAULT) continue;
                    pushinstack(stack, w);
                    new_cc[w] = parent_cc;
                    cc->updateIndex[w].visited[thread_id] = version;
                }
            }
        }

        parent_cc++;
    }
    deletestack(stack);

    // change the cc index
    for (k = 0 ; k < cc->cc_size ; k++) {

        old_cc = cc->cc_index[k].id;

        if (old_cc == DEFAULT) continue;

        cc->cc_index[k].id = new_cc[old_cc];
    }
    free(new_cc);

    // delete the update index
    for (i = 0 ; i < cc->u_size ; i++) {
        free(cc->updateIndex[i].cc_array);
        free(cc->updateIndex[i].new_nodes);
    }

    free(cc->updateIndex);
    cc->updateIndex = NULL;

    cc->u_size = parent_cc;

    // initialize a new update index
    initUpdateIndex(cc);

    return parent_cc;
}

// Free CC index
void destroyCCIndex(CC *cc) {

    uint32_t i = 0;

    for (i = 0; i < cc->u_size; i++) {
        free(cc->updateIndex[i].cc_array);
        free(cc->updateIndex[i].new_nodes);
    }
    free(cc->updateIndex);
    free(cc->cc_index);
    free(cc);
}
