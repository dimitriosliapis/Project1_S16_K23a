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

uint32_t createCCIndex(uint32_t **cc_index, uint32_t cc_size, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in,
                       uint32_t size_out, ht_Node *explored) {

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

    for(i = 0; i < cc_size; i++){
        (*cc_index)[i] = DEFAULT;
    }

    cc_counter = 0;
    for (cur = 0; cur < size; cur++) {
        if((lookup(index_in, cur, size_in) == NOT_EXIST) && (lookup(index_out, cur, size_out) == NOT_EXIST)) continue;
        if (search(explored, cur, HT_BIG) == FOUND) continue; // visited
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);


            if (search(explored, v, HT_BIG) == NOT_FOUND) {
                (*cc_index)[v] = cc_counter;
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
    //delete(explored, HT_BIG);
    reinitialize(explored, HT_BIG);

    return cc_counter;
}

void refreshUpdateIndex(uint32_t *cc_index, uint32_t cc_size, u_node **updateIndex, uint32_t *update_index_size, uint32_t N1, uint32_t N2) {

    uint32_t cc[2];
    int found = 0;
    /*uint32_t cc2 = cc_index[N2];*/
    uint32_t new_cc = 0;
    uint32_t i = 0, realloc_size = 0, realloc_update_index_size = *update_index_size, k = 0, l = 0;
    uint32_t *temp = NULL;

    if(N1 >= cc_size) cc[0] = DEFAULT;
    else cc[0] = cc_index[N1];

    if(N2 >= cc_size) cc[1] = DEFAULT;
    else cc[1] = cc_index[N2];

    if(cc[0] != DEFAULT && cc[1] != DEFAULT && cc[0] == cc[1]) return;// an einai sto idio sinexise

    if(cc[0] == DEFAULT && cc[1] == DEFAULT) {                          // an einai kainouria cc
        i = 0;
        //ti ginetai an kapoio einai hdh sto cc_index?
        while((*updateIndex)[i].state == 'o' && i < *update_index_size) i++; //vres tin kainouria thesi

        if(i < *update_index_size) {

            l = i;
            while((*updateIndex)[l].state != 'e' && l < *update_index_size) l++;
            while(i < l){
                if((*updateIndex)[i].new_nodes != NULL){
                    for(k = 0; k < (*updateIndex)[i].n_size; k++){
                        if((*updateIndex)[i].new_nodes[k] == DEFAULT) break;
                        if((*updateIndex)[i].new_nodes[k] == N1){
                            cc[0] = i;
                            found = 1;
                        }
                        if((*updateIndex)[i].new_nodes[k] == N2){
                            cc[1] = i;
                            found = 1;
                        }
                    }
                }

                i++;
            }
            if(!found){
                if(l < *update_index_size){
                    new_cc = l;
                }
                else{                                                                   //an de xwraei kane realloc
                    new_cc = *update_index_size;
                    realloc_update_index_size = 2 * (*update_index_size);
                    *updateIndex = realloc(*updateIndex, realloc_update_index_size * sizeof(u_node));

                    for (i = (new_cc + 1); i < realloc_update_index_size; i++) {
                        (*updateIndex)[i].cc_array = NULL;
                        (*updateIndex)[i].size = 0;
                        (*updateIndex)[i].state = 'e';
                        (*updateIndex)[i].new_nodes = NULL;
                        (*updateIndex)[i].n_size = 0;
                    }
                    *update_index_size = realloc_update_index_size;
                }
                (*updateIndex)[new_cc].state = 'n';
                (*updateIndex)[new_cc].n_size = INIT_NEWNODE_SIZE;
                (*updateIndex)[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                (*updateIndex)[new_cc].new_nodes[0] = N1;
                (*updateIndex)[new_cc].new_nodes[1] = N2;
                for(k = 2; k < INIT_NEWNODE_SIZE; k++) (*updateIndex)[new_cc].new_nodes[k] = DEFAULT;

                return;

            }
            else{
                if(cc[0] == cc[1]) return;
            }
        }
        else{
            realloc_size = 2 * (*update_index_size);
            (*updateIndex) = realloc((*updateIndex),realloc_size * sizeof(u_node));
            new_cc = *update_index_size;
            (*updateIndex)[new_cc].state = 'n';
            (*updateIndex)[new_cc].n_size = INIT_NEWNODE_SIZE;
            (*updateIndex)[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
            (*updateIndex)[new_cc].new_nodes[0] = N1;
            (*updateIndex)[new_cc].new_nodes[1] = N2;
            for(k = 2; k < INIT_NEWNODE_SIZE; k++) (*updateIndex)[new_cc].new_nodes[k] = DEFAULT;
            for(l = new_cc + 1; l < realloc_size; l++){
                (*updateIndex)[l].new_nodes = NULL;
                (*updateIndex)[l].state = 'e';
                (*updateIndex)[l].n_size = 0;
                (*updateIndex)[l].cc_array = NULL;
                (*updateIndex)[l].size = 0;
            }
            (*update_index_size) = realloc_size;
            return;
        }

    }
    if(cc[0] == DEFAULT) {
        if((*updateIndex)[cc[1]].new_nodes == NULL){
            (*updateIndex)[cc[1]].n_size = INIT_NEWNODE_SIZE;
            (*updateIndex)[cc[1]].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
            (*updateIndex)[cc[1]].new_nodes[0] = N1;
        }
        else{
            k = 0;
            while((*updateIndex)[cc[1]].new_nodes[k] != DEFAULT && k < (*updateIndex)[cc[1]].n_size) k++;
            if(k == (*updateIndex)[cc[1]].n_size){
                realloc_size = 2 * (*updateIndex)[cc[1]].n_size;
                (*updateIndex)[cc[1]].new_nodes = realloc((*updateIndex)[cc[1]].new_nodes, realloc_size * sizeof(uint32_t));
                for(l = (*updateIndex)[cc[1]].n_size + 1; l < realloc_size; l++) (*updateIndex)[cc[1]].new_nodes[l] = DEFAULT;
            }
            (*updateIndex)[cc[1]].new_nodes[k] = N1;
        }
        return;
    }
    else if(cc[1] == DEFAULT){
        if((*updateIndex)[cc[0]].new_nodes == NULL){
            (*updateIndex)[cc[0]].n_size = INIT_NEWNODE_SIZE;
            (*updateIndex)[cc[0]].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
            (*updateIndex)[cc[0]].new_nodes[0] = N2;
        }
        else{
            k = 0;
            while((*updateIndex)[cc[0]].new_nodes[k] != DEFAULT && k < (*updateIndex)[cc[0]].n_size) k++;
            if(k == (*updateIndex)[cc[0]].n_size){
                realloc_size = 2 * (*updateIndex)[cc[0]].n_size;
                (*updateIndex)[cc[0]].new_nodes = realloc((*updateIndex)[cc[0]].new_nodes, realloc_size * sizeof(uint32_t));
                for(l = (*updateIndex)[cc[0]].n_size + 1; l < realloc_size; l++) (*updateIndex)[cc[0]].new_nodes[l] = DEFAULT;
            }
            (*updateIndex)[cc[0]].new_nodes[k] = N2;
        }
        return;
    }

    int j = 0;

    //if(cc[0] == cc[1]) j = 1;//an einai kai ta 2 kainouria tote kanto 1 fora apla gia na dimiourgiseis kainourio cc

    while(j < 2) {

        if ((*updateIndex)[cc[j]].cc_array == NULL) {       //an den exei sindethei me alla cc (o pinakas twn cc einai kenos)
            (*updateIndex)[cc[j]].cc_array = malloc(INIT_UNODE_SIZE * sizeof(uint32_t));
            temp = (*updateIndex)[cc[j]].cc_array;
            if(j == 0) temp[0] = cc[1];             //elegxos se poio apo ta 2 vriskomaste ligo mpakalistikos alla klain
            else temp[0] = cc[0];
            (*updateIndex)[cc[j]].size = INIT_UNODE_SIZE;
            i = 1;
            while (i < INIT_UNODE_SIZE) {
                temp[i] = DEFAULT;
                i++;
            }
        } else {
            temp = (*updateIndex)[cc[j]].cc_array;
            i = 0;
            while (temp[i] != DEFAULT && i < (*updateIndex)[cc[j]].size) i++;
            if (i < (*updateIndex)[cc[j]].size) {
                if(j == 0) temp[i] = cc[1];
                else temp[i] = cc[0];
            }
            else if (i == (*updateIndex)[cc[j]].size) {
                realloc_size = 2 * (*updateIndex)[cc[j]].size;
                uint32_t *new = NULL;
                new = realloc((*updateIndex)[cc[j]].cc_array, realloc_size * sizeof(uint32_t));
                if(j == 0) new[i] = cc[1];
                else new[i] = cc[0];
                for (i = (*updateIndex)[cc[j]].size + 1; i < realloc_size; i++) new[i] = DEFAULT;
                (*updateIndex)[cc[j]].size = realloc_size;
                (*updateIndex)[cc[j]].cc_array = new;
            }
        }
        j++;
    }
}

int searchUpdateIndex(uint32_t *cc_index, uint32_t cc_size, u_node *updateIndex, uint32_t update_index_size, uint32_t N1, uint32_t N2, ht_Node *explored) {

    uint32_t cc1 = 0;
    uint32_t cc2 = 0;
    uint32_t v = 0;
    uint32_t *temp = NULL;
    uint32_t i = 0, j = 0;
    Stack stack;
    stack.last = NULL;

    if(N1 >= cc_size) cc1 = DEFAULT;
    else cc1 = cc_index[N1];
    if(N2 >= cc_size) cc2 = DEFAULT;
    else cc2 = cc_index[N2];

    if(cc1 == DEFAULT){
        i = 0;
        while(updateIndex[i].state == 'o' && i < update_index_size) i++;
        if(i == update_index_size || updateIndex[i].state == 'e') return NOT_FOUND;
        while(i < update_index_size){
            if(updateIndex[i].new_nodes != NULL){
                for(j = 0; j < updateIndex[i].n_size; j++){
                    if(updateIndex[i].new_nodes[j] == N1) {
                        cc1 = i;
                        break;
                    }
                }
            }
            i++;
        }
    }
    if(cc2 == DEFAULT){
        i = 0;
        while(updateIndex[i].state == 'o' && i < update_index_size) i++;
        if(i == update_index_size || updateIndex[i].state == 'e') return NOT_FOUND;
        while(i < update_index_size){
            if(updateIndex[i].new_nodes != NULL){
                for(j = 0; j < updateIndex[i].n_size; j++){
                    if(updateIndex[i].new_nodes[j] == N2) {
                        cc2 = i;
                        break;
                    }
                }
            }
            i++;
        }
    }
    if(cc1 == DEFAULT || cc2 == DEFAULT) return NOT_FOUND;

    if(cc1 != cc2) {
        push(&stack, cc1);
        while(!stackIsEmpty(&stack)) {
            v = pop(&stack);
            if(v != cc2) {
                if (search(explored, v, HT_BIG) == NOT_FOUND) {
                    insert(explored, v, HT_BIG);

                    temp = updateIndex[v].cc_array;
                    if(temp == NULL) {
                        continue;//de tha prepe na nai NULL
                    }

                    i = 0;
                   // update_node_size = sizeof(updateIndex[v])/sizeof(uint32_t);
                    while(temp[i] != DEFAULT && i < updateIndex[v].size) {
                        push(&stack, temp[i]);
                        i++;
                    }
                }
            }
            else {
                reinitialize(explored, HT_BIG);
                return FOUND;
            }
        }
        reinitialize(explored, HT_BIG);
        return NOT_FOUND;
    }
    else{
        reinitialize(explored, HT_BIG);
        return FOUND;
    }
}

uint32_t findCCMax(uint32_t *cc_index, uint32_t cc_index_size){

    uint32_t i = 0;
    uint32_t max = 0;

    for(i = 0; i < cc_index_size; i++){
        if(cc_index[i] == DEFAULT) break;
        if(cc_index[i] > max) max = cc_index[i];
    }

    return max;
}


void updateCCIndex(uint32_t *cc_index, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in,
                  uint32_t size_out, u_node *updateIndex, uint32_t *cc_index_size, uint32_t update_index_size) {

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


    for(i = 0; i < update_index_size; i++ ) {

        parent_cc = i;

        if(updateIndex[i].state == 'e') break;
        if(updateIndex[i].state != 'v'){          //visited

            if(updateIndex[i].state == 'n'){

            }
            if (search(explored, parent_cc, HT_BIG) == FOUND) continue;

            push(&stack, parent_cc);

            while (!stackIsEmpty(&stack)) {

                v = pop(&stack);
                updateIndex[v].state = 'v';
                //cc_index[v] = parent_cc;

                insert(explored, v, HT_BIG);

                for (j = 0; j < updateIndex[v].size; j++) {
                    push(&stack, updateIndex[v].cc_array[j]);
                }

            }

            for(j = 0; j < *cc_index_size; j++){
                if (search(explored, cc_index[j], HT_BIG) == FOUND){
                    cc_index[j] = parent_cc;
                }

            }
        }



        /*parent_cc = cc_index[i];
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

        }*/
    }
    delete(explored, HT_BIG);
    delete(explored_new, HT_BIG);

}