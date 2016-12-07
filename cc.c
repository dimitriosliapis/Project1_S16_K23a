#include "cc.h"


void push(Stack *stack, uint32_t id) {

    sNode *new = malloc(sizeof(sNode));

    new->id = id;
    if(stack->last == NULL) new->next = NULL;
    else new->next = stack->last;

    stack->last = new;
}

uint32_t pop(Stack *stack) {

    sNode *rem = stack->last;
    uint32_t id;

    if (stack->last == NULL) return DEFAULT;

    id = stack->last->id;
    if(stack->last->next == NULL) stack->last = NULL;
    else stack->last = stack->last->next;
    free(rem);
    return id;
}

int stackIsEmpty(Stack *stack) {
    if (stack == NULL) return 1;
    if (stack->last == NULL) return 1;
    return 0;
}

void deleteStack(Stack *stack) {

/*    if (stack == NULL) return;
    if (stack->last == NULL) {
        free(stack);
        return;
    }
    if (current->next == NULL) {
        free(current);
        return;
    }
    deleteStack(stack, current->next);*/
    sNode *cur = NULL;
    sNode *prev = NULL;

    cur = stack->last;
    prev = NULL;

    while(cur != NULL){
        prev = cur;
        cur = cur->next;
        free(prev);
    }
}

CC* createCCIndex(uint32_t cc_size, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in, uint32_t size_out, ht_Node *explored, uint32_t version) {

    Stack stack;
    uint32_t cur = 0;
    uint32_t v = 0;
    list_node *neighbors_in, *neighbors_out;
    uint32_t cc_counter = 0;
    ptrdiff_t offset_in, offset_out;
    uint32_t i = 0;
    uint32_t size;

    CC *cc = malloc(sizeof(CC));
    uint32_t *cc_index = malloc(sizeof(uint32_t) * cc_size);

    for(i = 0; i < cc_size; i++){
        cc_index[i] = DEFAULT;
    }

    cc->cc_index = cc_index;
    cc->cc_size = cc_size;


    if(size_in > size_out) size = size_in;
    else size = size_out;

    stack.last = NULL;



    cc_counter = 0;
    for (cur = 0; cur < size; cur++) {
        if((lookup(index_in, cur, size_in) == NOT_EXIST)  && (lookup(index_out, cur, size_out) == NOT_EXIST)) continue;
        if (search(explored, cur, HT_BIG, version) == FOUND) continue; // visited
        push(&stack, cur);
        while (!stackIsEmpty(&stack)) {
            v = pop(&stack);

            if (search(explored, v, HT_BIG, version) == NOT_FOUND) {
                cc_index[v] = cc_counter;
                insert(explored, v, HT_BIG, version);

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
                        if ((i == N ) && ( neighbors_in->nextListNode != -1)) {
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
                        if ((i == N ) && ( neighbors_out->nextListNode != -1)) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            i = 0;
                        }
                    }
                }
            }

        }
        cc_counter++;
    }

    cc->cc_max = cc_counter;
    deleteStack(&stack);

    return cc;
}

void initUpdateIndex(CC *cc){

    u_node *update_index = malloc(cc->u_size * sizeof(u_node));

    uint32_t a = 0;
    while(a < cc->u_size) {
        update_index[a].cc_array = NULL;
        update_index[a].new_nodes = NULL;
        update_index[a].size = 0;
        update_index[a].n_size = 0;
        update_index[a].state = 'o';//old
        a++;
    }

    cc->updateIndex = update_index;

}

void refreshUpdateIndex(CC *cc, uint32_t N1, uint32_t N2) {

    uint32_t cc1, cc2;
    int found = 0;
    uint32_t new_cc = 0;
    uint32_t i = 0, realloc_size = 0, realloc_update_index_size = cc->u_size, k = 0, l = 0;
    uint32_t *temp = NULL;


    if(N1 >= cc->cc_size) cc1 = DEFAULT;
    else cc1 = cc->cc_index[N1];

    if(N2 >= cc->cc_size) cc2 = DEFAULT;
    else cc2 = cc->cc_index[N2];

    if((cc1 != DEFAULT) && (cc1 == cc2)) return;// an einai sto idio sinexise

    if((cc1 == DEFAULT) && (cc2 == DEFAULT)) {                          // an einai kainouria cc
        i = 0;
        //ti ginetai an kapoio einai hdh sto cc_index?
        while((cc->updateIndex[i].state == 'o') && (i < cc->u_size)) i++; //vres tin kainouria thesi

        if(i < cc->u_size) {

            l = i;
            while((cc->updateIndex[l].state != 'e') && (l < cc->u_size)) l++;
            while(i < l){
                if(cc->updateIndex[i].new_nodes != NULL){
                    for(k = 0; k < cc->updateIndex[i].n_size; k++){
                        if(cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                        if(cc->updateIndex[i].new_nodes[k] == N1){
                            cc1 = i;
                            found = 1;
                        }
                        if(cc->updateIndex[i].new_nodes[k] == N2){
                            cc2 = i;
                            found = 1;
                        }
                    }
                }

                i++;
            }
            if(!found){
                if(l < cc->u_size){
                    new_cc = l;
                }
                else{                                                                   //an de xwraei kane realloc
                    new_cc = cc->u_size;
                    realloc_update_index_size = 2 * cc->u_size;
                    cc->updateIndex = realloc(cc->updateIndex, realloc_update_index_size * sizeof(u_node));

                    for (i = (new_cc + 1); i < realloc_update_index_size; i++) {
                        cc->updateIndex[i].cc_array = NULL;
                        cc->updateIndex[i].size = 0;
                        cc->updateIndex[i].state = 'e';
                        cc->updateIndex[i].new_nodes = NULL;
                        cc->updateIndex[i].n_size = 0;
                    }
                    cc->u_size = realloc_update_index_size;
                }
                cc->updateIndex[new_cc].state = 'n';
                cc->updateIndex[new_cc].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[new_cc].new_nodes[0] = N1;
                cc->updateIndex[new_cc].new_nodes[1] = N2;
                for(k = 2; k < INIT_NEWNODE_SIZE; k++) cc->updateIndex[new_cc].new_nodes[k] = DEFAULT;

                return;

            }
            else{
                if(cc1 == cc2) return;
            }
        }
        else{
            realloc_size = 2 * cc->u_size;
            cc->updateIndex = realloc(cc->updateIndex,realloc_size * sizeof(u_node));
            new_cc = cc->u_size;
            cc->updateIndex[new_cc].state = 'n';
            cc->updateIndex[new_cc].n_size = INIT_NEWNODE_SIZE;
            cc->updateIndex[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
            cc->updateIndex[new_cc].new_nodes[0] = N1;
            cc->updateIndex[new_cc].new_nodes[1] = N2;
            for(k = 2; k < INIT_NEWNODE_SIZE; k++) cc->updateIndex[new_cc].new_nodes[k] = DEFAULT;
            for(l = new_cc + 1; l < realloc_size; l++){
                cc->updateIndex[l].new_nodes = NULL;
                cc->updateIndex[l].state = 'e';
                cc->updateIndex[l].n_size = 0;
                cc->updateIndex[l].cc_array = NULL;
                cc->updateIndex[l].size = 0;
            }
            cc->u_size = realloc_size;
            return;
        }

    }
    if(cc1 == DEFAULT) {

        i = 0;
        found = 0;
        while((cc->updateIndex[i].state == 'o') && (i < cc->u_size)) i++; //vres tin kainouria thesi

        if(i < cc->u_size) {

            l = i;
            while ((cc->updateIndex[l].state != 'e') && (l < cc->u_size)) l++;
            while (i < l) {
                if (cc->updateIndex[i].new_nodes != NULL) {
                    for (k = 0; k < cc->updateIndex[i].n_size; k++) {
                        if (cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                        if (cc->updateIndex[i].new_nodes[k] == N1) {
                            cc1 = i;
                            found = 1;
                        }
                    }
                }

                i++;
            }
        }
        if(!found) {
            if (cc->updateIndex[cc2].new_nodes == NULL) {
                cc->updateIndex[cc2].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[cc2].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[cc2].new_nodes[0] = N1;
                for (k = 1; k < INIT_NEWNODE_SIZE; k++) cc->updateIndex[cc2].new_nodes[k] = DEFAULT;
            }
            else {
                k = 0;
                while ((cc->updateIndex[cc2].new_nodes[k] != DEFAULT ) && ( k < cc->updateIndex[cc2].n_size)) k++;
                if (k == cc->updateIndex[cc2].n_size) {
                    realloc_size = 2 * cc->updateIndex[cc2].n_size;
                    cc->updateIndex[cc2].new_nodes = realloc(cc->updateIndex[cc2].new_nodes,
                                                              realloc_size * sizeof(uint32_t));
                    for (l = cc->updateIndex[cc2].n_size + 1; l < realloc_size; l++)
                        cc->updateIndex[cc2].new_nodes[l] = DEFAULT;
                }
                cc->updateIndex[cc2].new_nodes[k] = N1;
            }
            return;
        }
    }
    else if(cc2 == DEFAULT){

        found = 0;
        i = 0;
        while((cc->updateIndex[i].state == 'o' ) && ( i < cc->u_size)) i++; //vres tin kainouria thesi

        if(i < cc->u_size) {

            l = i;
            while ((cc->updateIndex[l].state != 'e' ) && ( l < cc->u_size)) l++;
            while (i < l) {
                if (cc->updateIndex[i].new_nodes != NULL) {
                    for (k = 0; k < cc->updateIndex[i].n_size; k++) {
                        if (cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                        if (cc->updateIndex[i].new_nodes[k] == N2) {
                            cc2 = i;
                            found = 1;
                        }
                    }
                }

                i++;
            }
        }

        if(!found) {
            if (cc->updateIndex[cc1].new_nodes == NULL) {
                cc->updateIndex[cc1].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[cc1].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[cc1].new_nodes[0] = N2;
                for (k = 1; k < INIT_NEWNODE_SIZE; k++) cc->updateIndex[cc1].new_nodes[k] = DEFAULT;
            }
            else {
                k = 0;
                while ((cc->updateIndex[cc1].new_nodes[k] != DEFAULT ) && ( k < cc->updateIndex[cc1].n_size)) k++;
                if (k == cc->updateIndex[cc1].n_size) {
                    realloc_size = 2 * cc->updateIndex[cc1].n_size;
                    cc->updateIndex[cc1].new_nodes = realloc(cc->updateIndex[cc1].new_nodes,
                                                              realloc_size * sizeof(uint32_t));
                    for (l = cc->updateIndex[cc1].n_size + 1; l < realloc_size; l++)
                        cc->updateIndex[cc1].new_nodes[l] = DEFAULT;
                }
                cc->updateIndex[cc1].new_nodes[k] = N2;
            }
            return;
        }
    }

    int j = 0;
    uint32_t cur_cc = cc1;

    while(j < 2) {

        if (cc->updateIndex[cur_cc].cc_array == NULL) {       //an den exei sindethei me alla cc (o pinakas twn cc einai kenos)
            cc->updateIndex[cur_cc].cc_array = malloc(INIT_UNODE_SIZE * sizeof(uint32_t));
            temp = cc->updateIndex[cur_cc].cc_array;
            if(j == 0) temp[0] = cc2;             //elegxos se poio apo ta 2 vriskomaste ligo mpakalistikos alla klain
            else temp[0] = cc1;
            cc->updateIndex[cur_cc].size = INIT_UNODE_SIZE;
            i = 1;
            while (i < INIT_UNODE_SIZE) {
                temp[i] = DEFAULT;
                i++;
            }
        } else {
            temp = cc->updateIndex[cur_cc].cc_array;
            i = 0;
            while ((temp[i] != DEFAULT ) && ( i < cc->updateIndex[cur_cc].size)) i++;
            if (i < cc->updateIndex[cur_cc].size) {
                if(j == 0) temp[i] = cc2;
                else temp[i] = cc1;
            }
            else if (i == cc->updateIndex[cur_cc].size) {
                realloc_size = 2 * cc->updateIndex[cur_cc].size;
                cc->updateIndex[cur_cc].cc_array = realloc(cc->updateIndex[cur_cc].cc_array, realloc_size * sizeof(uint32_t));
                if(j == 0) cc->updateIndex[cur_cc].cc_array[i] = cc2;
                else cc->updateIndex[cur_cc].cc_array[i] = cc1;
                for (i = cc->updateIndex[cur_cc].size + 1; i < realloc_size; i++) cc->updateIndex[cur_cc].cc_array[i] = DEFAULT;
                cc->updateIndex[cur_cc].size = realloc_size;
               // cc->updateIndex[cur_cc].cc_array = new;
            }
        }
        j++;
        cur_cc = cc2;
    }
}

int searchUpdateIndex(CC cc, uint32_t N1, uint32_t N2, ht_Node *explored, uint32_t version) {

    uint32_t cc1 = 0;
    uint32_t cc2 = 0;
    uint32_t v = 0;
    uint32_t i = 0, j = 0;
    Stack stack;
    stack.last = NULL;

    if(N1 >= cc.cc_size) cc1 = DEFAULT;
    else cc1 = cc.cc_index[N1];
    if(N2 >= cc.cc_size) cc2 = DEFAULT;
    else cc2 = cc.cc_index[N2];

    if(cc1 == DEFAULT){
        i = 0;

        while(i < cc.u_size){
            if(i == cc.u_size || cc.updateIndex[i].state == 'e') return NOT_FOUND;
            if(cc.updateIndex[i].new_nodes != NULL){
                for(j = 0; j < cc.updateIndex[i].n_size; j++){
                    if(cc.updateIndex[i].new_nodes[j] == N1) {
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

        while(i < cc.u_size){
            if(i == cc.u_size || cc.updateIndex[i].state == 'e') return NOT_FOUND;
            if(cc.updateIndex[i].new_nodes != NULL){
                for(j = 0; j < cc.updateIndex[i].n_size; j++){
                    if(cc.updateIndex[i].new_nodes[j] == N2) {
                        cc2 = i;
                        break;
                    }
                }
            }
            i++;
        }
    }
    if(cc1 == DEFAULT || cc2 == DEFAULT) {
        deleteStack(&stack);
        return NOT_FOUND;
    }

    if(cc1 != cc2) {
        push(&stack, cc1);
        while(!stackIsEmpty(&stack)) {
            v = pop(&stack);
            if(v != cc2) {
                if (search(explored, v, HT_BIG, version) == NOT_FOUND) {
                    insert(explored, v, HT_BIG, version);

                    //temp = cc.updateIndex[v].cc_array;
                    if(cc.updateIndex[v].cc_array == NULL) {
                        continue;//de tha prepe na nai NULL
                    }

                    i = 0;

                    while((cc.updateIndex[v].cc_array[i] != DEFAULT ) && ( i < cc.updateIndex[v].size)) {
                        push(&stack, cc.updateIndex[v].cc_array[i]);
                        i++;
                    }
                }
            }
            else {
                deleteStack(&stack);
                return FOUND;
            }
        }
        deleteStack(&stack);
        return NOT_FOUND;
    }
    else{
        deleteStack(&stack);
        return FOUND;
    }
}

uint32_t updateCCIndex(CC *cc, ht_Node* explored, ht_Node* explored_new, uint32_t version, uint32_t new_size) {

    uint32_t v = 0, w = 0;
    uint32_t i = 0, j = 0;
    Stack stack;
    uint32_t k;
    uint32_t parent_cc = 0;
    //uint32_t version_new = 0;
    uint32_t new_cc[cc->u_size];
    uint32_t old_cc = 0;

    for(i = 0; i < cc->u_size; i++) new_cc[i] = DEFAULT;

    stack.last = NULL;

    if(new_size > cc->cc_size){
        cc->cc_index = realloc(cc->cc_index, new_size*sizeof(uint32_t));
        for(j = cc->cc_size; j < new_size; j++){
            cc->cc_index[j] = DEFAULT;
        }
    }

    //gia ta new nodes
    for(i = 0; i < cc->u_size; i++ ) {
        if(cc->updateIndex[i].new_nodes != NULL) {
            for(k = 0 ; k < cc->updateIndex[i].n_size ; k++) {
                if(cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                cc->cc_index[cc->updateIndex[i].new_nodes[k]] = i;
            }
        }
    }
    parent_cc = 0;//gia na bgoun me ti seira ta CC
    for(i = 0 ; i < cc->u_size ; i++) {
        if(cc->updateIndex[i].state == 'e') break;
        if(search(explored, i, HT_BIG, version) == FOUND) continue;
        push(&stack, i);
        //insert(explored_new, i, HT_BIG, version_new);
        new_cc[i] = parent_cc;
        insert(explored, i, HT_BIG, version);
        while(!stackIsEmpty(&stack)) {
            v = pop(&stack);

            if(cc->updateIndex[v].cc_array != NULL) {
                for (k = 0; k < cc->updateIndex[v].size; k++) {
                    w = cc->updateIndex[v].cc_array[k];
                    if(w == DEFAULT) break;
                    if(new_cc[w] != DEFAULT) continue;
                   // if(search(explored_new, w, HT_BIG, version_new) == FOUND) continue;
                    push(&stack, w);
                    //insert(explored_new, w, HT_BIG, version_new);
                    new_cc[w] = parent_cc;
                    insert(explored, w, HT_BIG, version);
                }
            }

        }
        parent_cc++;
    }
    for(k = 0 ; k < cc->cc_size ; k++) {
        old_cc = cc->cc_index[k];
        if(old_cc == DEFAULT) continue;
        cc->cc_index[k] = new_cc[old_cc];
    }
    deleteStack(&stack);
    for(i = 0; i < cc->u_size; i++){
        free(cc->updateIndex[i].cc_array);
        free(cc->updateIndex[i].new_nodes);

        cc->updateIndex[i].cc_array = NULL;
        cc->updateIndex[i].new_nodes = NULL;
    }
    free(cc->updateIndex);
    cc->updateIndex = NULL;

    cc->u_size = parent_cc;

    initUpdateIndex(cc);

    return parent_cc;
}

void destroyCCIndex(CC *cc){

    uint32_t i = 0;

    for(i = 0; i < cc->u_size; i++){
        free(cc->updateIndex[i].cc_array);
        free(cc->updateIndex[i].new_nodes);
    }
    free(cc->updateIndex);
    free(cc->cc_index);
    free(cc);
    cc = NULL;
}