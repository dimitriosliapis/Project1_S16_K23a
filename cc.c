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


/*
 * ksekinaw ap tin arxi tou index pairnw ton kathe komvo tsekarw an einai visited
 * an den einai kanw DFS kai vazw olous tous geitones tou sto CC
 * an einai proxoraw ston epomeno
*/
uint32_t createCCIndex(uint32_t *cc_index, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in,
             uint32_t size_out) {

    ht_Node *explored = createHashtable(HT_BIG);
    Stack stack;
    uint32_t cur = 0;
    uint32_t v = 0;
    list_node *neighbors;
    uint32_t cc_size;
    uint32_t cc_counter = 0;
    ind *cur_ind = NULL;
    ptrdiff_t offset;
    int i = 0;

    stack.last = NULL;

    if (size_in > size_out) {
        cc_size = size_in;
    }
    else {
        cc_size = size_out;
    }


    cc_counter = 0;
    for (cur = 0; cur < size_in; cur++) {
        if (search(explored, cur, HT_BIG) == FOUND) continue; // visited
        insert(explored, cur, HT_BIG);
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);
            cc_index[v] = cc_counter;

            offset = getListHead(index_in, v);
            if(offset < 0) continue;
            neighbors = buffer_in + offset;

            while (i < N) {

                if (neighbors->neighbor[i] == DEFAULT) break;
                if (search(explored, neighbors->neighbor[i], HT_BIG) == NOT_FOUND) {
                    insert(explored, neighbors->neighbor[i], HT_BIG);
                    push(&stack, neighbors->neighbor[i]);
                }

                i++;
                if (i == N && neighbors->nextListNode != -1) {
                    neighbors = buffer_in + neighbors->nextListNode;
                    i = 0;
                }
            }

        }
        cc_counter++;
    }

    for (cur = 0; cur < size_out; cur++) {
        if (search(explored, cur, HT_BIG) == FOUND) continue; // visited
        insert(explored, cur, HT_BIG);
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);
            cc_index[v] = cc_counter;

            offset = getListHead(index_out, v);
            if(offset < 0) continue;
            neighbors = buffer_out + offset;

            while (i < N) {

                if (neighbors->neighbor[i] == DEFAULT) break;
                if (search(explored, neighbors->neighbor[i], HT_BIG) == NOT_FOUND) {
                    insert(explored, neighbors->neighbor[i], HT_BIG);
                    push(&stack, neighbors->neighbor[i]);
                }

                i++;
                if (i == N && neighbors->nextListNode != -1) {
                    neighbors = buffer_out + neighbors->nextListNode;
                    i = 0;
                }
            }

        }
        cc_counter++;
    }

    return cc_counter;

}