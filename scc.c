#include "scc.h"
#include "cc.h"

uint32_t peek(Stack *stack) {

    return stack->last->id;

}

void tarjan(ind *index_out, list_node *buffer_out, uint32_t size_out) {

    Stack stack;
    ht_Node *explored = createHashtable(HT_BIG);
    list_node *neighbors_out;
    ptrdiff_t offset_out;
    uint32_t size = size_out, index = 0;
    uint32_t i = 0, v = 0, k = 0, a = 0;

    SCC *scc = malloc(sizeof(SCC));

    stack.last = NULL;

    for(i = 0 ; i < size ; i++) {
        if(lookup(index_out, i, size_out) == NOT_EXIST) continue;
        if(search(explored, i, HT_BIG) == FOUND) continue;

        push(&stack, i);
        while(!stackIsEmpty(&stack)) {
            v = peek(&stack);

            index_out[v].lowlink = index;
            index_out[v].index = index;
            index++;

            if(search(explored, v, HT_BIG) == NOT_FOUND) {
                insert(explored, v, HT_BIG);
                offset_out = getListHead(index_out, v);

                if(offset_out >= 0) {
                    neighbors_out = buffer_out + offset_out;
                    k = 0;
                    while(k < N) {
                        if(neighbors_out->neighbor[k] == DEFAULT) break;
                        if(search(explored, neighbors_out->neighbor[k], HT_BIG) == NOT_FOUND) {
                            push(&stack, neighbors_out->neighbor[k]);
                            if(index_out[v].lowlink > index_out[k].lowlink) index_out[v].lowlink = index_out[k].lowlink;
                        }
                        else if(onStack(&stack, k)){
                            if(index_out[v].lowlink > index_out[k].index) index_out[v].lowlink = index_out[k].index;
                        }
                        k++;
                        if(k == N && neighbors_out->nextListNode != -1) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            k = 0;
                        }
                    }
                }
                else  {
                    if (index_out[v].lowlink == index_out[v].index) {
                        scc->components[i].component_id = i;
                        while (k != v) {
                            k = pop(&stack);
                            scc->components[i].included_node_ids[a] = k;
                            scc->components[i].included_nodes_count++;
                            a++;
                        }
                        a = 0;
                    }
                }
            }
        }
    }
}