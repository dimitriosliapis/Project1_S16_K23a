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
    list_node *neighbors;
    uint32_t cc_counter = 0;
    ind *cur_ind = NULL;
    ptrdiff_t offset;
    int i = 0;

    stack.last = NULL;

/*    1  procedure DFS-iterative(G,v):
    2      let S be a stack
    3      S.push(v)
    4      while S is not empty
    5          v = S.pop()
    6          if v is not labeled as discovered:
    7              label v as discovered
    8              for all edges from v to w in G.adjacentEdges(v) do
    9                  S.push(w)
*/

    cc_counter = 0;
    for (cur = 0; cur < size_in; cur++) {
        if(lookup(index_in, cur, size_in) == NOT_EXIST) continue;
        if (search(explored, cur, HT_BIG) == FOUND) continue; // visited
        //insert(explored, cur, HT_BIG);
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);
            cc_index[v] = cc_counter;

            if (search(explored, v, HT_BIG) == NOT_FOUND) {
                insert(explored, v, HT_BIG);

                offset = getListHead(index_in, v);
                if(offset < 0) continue;
                neighbors = buffer_in + offset;

                i = 0;
                while (i < N) {
                    if (neighbors->neighbor[i] == DEFAULT) break;
                    push(&stack, neighbors->neighbor[i]);
                    i++;
                    if (i == N && neighbors->nextListNode != -1) {
                        neighbors = buffer_in + neighbors->nextListNode;
                        i = 0;
                    }
                }
            }

        }
        cc_counter++;
    }

    for (cur = 0; cur < size_out; cur++) {
        if(lookup(index_in, cur, size_out) == NOT_EXIST) continue;
        if (search(explored, cur, HT_BIG) == FOUND) continue; // visited
        //insert(explored, cur, HT_BIG);
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);
            cc_index[v] = cc_counter;

            if (search(explored, v, HT_BIG) == NOT_FOUND) {
                insert(explored, v, HT_BIG);

                offset = getListHead(index_out, v);
                if(offset < 0) continue;
                neighbors = buffer_out + offset;

                i = 0;
                while (i < N) {
                    if (neighbors->neighbor[i] == DEFAULT) break;
                    push(&stack, neighbors->neighbor[i]);
                    i++;
                    if (i == N && neighbors->nextListNode != -1) {
                        neighbors = buffer_out + neighbors->nextListNode;
                        i = 0;
                    }
                }
            }

        }
        cc_counter++;
    }
    return cc_counter;
}

int CreateUpdateIndex(uint32_t *cc_index, uint32_t **updateIndex, int update_node_size, int *update_index_size, uint32_t N1, uint32_t N2) {

    uint32_t cc1 = cc_index[N1];
    uint32_t cc2 = cc_index[N2];
    int i = 0, realloc_size = 0, realloc_update_index_size = *update_index_size;
    uint32_t *temp = NULL;

    if(N1 < *update_index_size && N2 < *update_index_size) {
        if (cc1 != cc2) {
            //gia to N1
            if (updateIndex[cc1] == NULL) {
                updateIndex[cc1] = malloc(update_node_size * sizeof(uint32_t));
                temp = updateIndex[N1];
                temp[0] = cc2;
                i = 1;
                while (i < update_node_size) {
                    temp[i] = DEFAULT;
                    i++;
                }
                printf("eftiaksa to %d\n", cc1);
            } else {
                temp = updateIndex[cc1];
                i = 0;
                while (temp[i] != DEFAULT || i < update_node_size) i++;
                printf("twra pairnaw?\n");
                if (i < update_node_size) temp[i] = cc2;
                else if (i == update_node_size) {
                    realloc_size = 2 * update_node_size;
                    uint32_t *new = NULL;
                    new = realloc(updateIndex[cc1], realloc_size * sizeof(uint32_t));
                    new[i] = cc2;
                    for (i = update_node_size; i < realloc_size; i++) new[i] = DEFAULT;
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
                printf("eftiaksa to %d\n", cc2);
                return update_node_size;
            } else {
                i = 1;
                temp = updateIndex[N2];
                while (temp[i] != DEFAULT && i < update_node_size) i++;
                if (i < update_node_size) temp[i] = cc1;
                else if (i == update_node_size) {
                    realloc_size = 2 * update_node_size;
                    uint32_t *new2 = NULL;
                    new2 = realloc(updateIndex[cc2], realloc_size * sizeof(uint32_t));
                    new2[i] = cc1;
                    for (i = update_node_size; i < realloc_size; i++) new2[i] = DEFAULT;
                    updateIndex[cc2] = new2;
                    update_node_size = realloc_size;
                    return update_node_size;
                }
            }
        }
    }
    else {
        //ama to index den xwraei
        while((realloc_update_index_size < cc1) || (realloc_update_index_size < cc2)) realloc_update_index_size = realloc_update_index_size * 2;
        updateIndex = realloc(updateIndex, realloc_update_index_size * sizeof(uint32_t *));
        for (i = *update_index_size; i < realloc_update_index_size; i++) updateIndex[i] = NULL;
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
            printf("eftiaksa to %d\n", cc1);
        } else {
            temp = updateIndex[cc1];
            while (temp[i] != DEFAULT && i < update_node_size) i++;
            if (i < update_node_size) temp[i] = cc2;
            else if (i == update_node_size) {
                realloc_size = 2 * update_node_size;
                uint32_t *new = NULL;
                new = realloc(updateIndex[cc1], realloc_size * sizeof(uint32_t));
                new[i] = cc2;
                for (i = update_node_size; i < realloc_size; i++) new[i] = DEFAULT;
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
            printf("eftiaksa to %d\n", cc2);
            return update_node_size;
        } else {
            i = 1;
            temp = updateIndex[cc2];
            while (temp[i] != DEFAULT && i < update_node_size) i++;
            if (i < update_node_size) updateIndex[cc2][i] = cc1;
            else if (i == update_node_size) {
                realloc_size = 2 * update_node_size;
                uint32_t *new2 = NULL;
                new2 = realloc(updateIndex[cc2], realloc_size * sizeof(uint32_t));
                new2[i] = cc1;
                for (i = update_node_size; i < realloc_size; i++) new2[i] = DEFAULT;
                updateIndex[cc2] = new2;
                update_node_size = realloc_size;
                return update_node_size;
            }
        }
    }
}