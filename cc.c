#include "cc.h"

Stack_t *createStack() {

    int i = 0;
    Stack_t *stack = NULL;
    stack = malloc(sizeof(Stack_t));

    if(stack == NULL) return NULL;
    else {
        stack->size = STACK_ARRAY_SIZE;
        stack->stack_array = malloc(STACK_ARRAY_SIZE*sizeof(uint32_t));
        for(i = 0 ; i < STACK_ARRAY_SIZE ; i++) stack->stack_array[i] = DEFAULT;
        stack->first = 0;
        stack->last = -1;
        stack->count = 0;
        return stack;
    }
}

int stackisempty(Stack_t *stack) {

    if(stack->count == 0) return 1;
    else return 0;

}

void pushinstack(Stack_t *stack, uint32_t id) {

    if(stack->count == stack->size) {
        stack->stack_array = realloc(stack->stack_array, 2*stack->size*sizeof(uint32_t));
        stack->size = 2*stack->size;
        int i = 0;
        for(i = stack->size/2 ; i < stack->size ; i++) stack->stack_array[i] = DEFAULT;
    }
    stack->last = (stack->last + 1) % stack->size;
    stack->stack_array[stack->last] = id;
    stack->count++;
}

uint32_t popfromstack(Stack_t *stack) {

    uint32_t id = DEFAULT;
    if(stack->count == 0) return DEFAULT;

    id = stack->stack_array[stack->last];
    stack->stack_array[stack->last] = DEFAULT;
    stack->last = (stack->last - 1) % stack->size;
    stack->count--;
    return id;
}

uint32_t peekfromstack(Stack_t *stack) {

    if(stack->stack_array[stack->last] == DEFAULT) return DEFAULT;

    uint32_t id = DEFAULT;
    id = stack->stack_array[stack->last];
    return id;
}

void deletestack(Stack_t *stack) {
    free(stack->stack_array);
    free(stack);
}

//////////////////////////////////////////////////////////////////////////

/*void push(Stack *stack, uint32_t id) {

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

    sNode *cur = NULL;
    sNode *prev = NULL;

    cur = stack->last;
    prev = NULL;

    while(cur != NULL){
        prev = cur;
        cur = cur->next;
        free(prev);
    }
}*/

CC* createCCIndex(uint32_t cc_size, ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t size_in, uint32_t size_out, uint32_t version) {

    //dimiourgia CCIndex

    Stack_t *stack = createStack();
    uint32_t cur = 0;
    uint32_t v = 0;
    list_node *neighbors_in, *neighbors_out;
    uint32_t cc_counter = 0;
    ptrdiff_t offset_in, offset_out;
    uint32_t i = 0, j = 0;
    int in, out;


    //memory allocation + initialise
    CC *cc = malloc(sizeof(CC));

    CC_ind *cc_index = malloc(sizeof(CC_ind) * cc_size);

    for(i = 0; i < cc_size; i++){
        cc_index[i].id = DEFAULT;
        for(j = 0; j <= THREAD_POOL_SIZE; j++) cc_index[i].visited[j] = DEFAULT;
    }

    cc->cc_index = cc_index;

    cc->cc_size = cc_size;

  //  stack.last = NULL;


    //metritis cc
    cc_counter = 0;

    for (cur = 0; cur < cc_size; cur++) {
        in = lookup(index_in, cur, size_in);
        out = lookup(index_out, cur, size_out);
        if((in == NOT_EXIST) && (out == NOT_EXIST)) continue;
        //if((lookup(index_in, cur, size_in) == NOT_EXIST)  && (lookup(index_out, cur, size_out) == NOT_EXIST)) continue;//an o komvos den uparxei sinexise ston epomeno
        //if (search(explored, cur, HT_BIG, version) == FOUND) continue; // to idio an einai visited
        if((in == ALR_EXISTS) && (index_in[cur].visited[0] == version)) continue;
        if((out == ALR_EXISTS) && (index_out[cur].visited[0] == version)) continue;


        //vale sto stack ton komvo kai olous tous geitones tou kai kane to idio gia tous geitones tous klp...
        //oloi autoi anhkoun sto idio cc
        pushinstack(stack, cur);
        while (!stackisempty(stack)) {
            v = popfromstack(stack);

            in = lookup(index_in, v, size_in);
            out = lookup(index_out, v, size_out);
            //if (search(explored, v, HT_BIG, version) == NOT_FOUND) {
            if(((in == ALR_EXISTS) && (index_in[v].visited[0] != version)) || ((out == ALR_EXISTS) && (index_out[v].visited[0] != version))){
                cc_index[v].id = cc_counter;
                //insert(explored, v, HT_BIG, version);
                if(in == ALR_EXISTS) index_in[v].visited[0] = version;
                if(out == ALR_EXISTS) index_out[v].visited[0] = version;



                offset_in = getListHead(index_in, v);
                offset_out = getListHead(index_out, v);
                neighbors_in = buffer_in + offset_in;
                neighbors_out = buffer_out + offset_out;

                if(offset_in >= 0) {
                    i = 0;
                    while (i < N) {
                        if (neighbors_in->neighbor[i] == DEFAULT) break;
                        pushinstack(stack, neighbors_in->neighbor[i]);
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
                        pushinstack(stack, neighbors_out->neighbor[i]);
                        i++;
                        if ((i == N ) && ( neighbors_out->nextListNode != -1)) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            i = 0;
                        }
                    }
                }
            }

        }
        cc_counter++;//epomeno cc
    }

    cc->cc_max = cc_counter;//otan teleiwsei krataei to teleutaio cc pou dimiourgise
    deletestack(stack);

    return cc;
}
//initialise update index
void initUpdateIndex(CC *cc){

    u_node *update_index = malloc(cc->u_size * sizeof(u_node));

    uint32_t a = 0;
    uint32_t j = 0;
    while(a < cc->u_size) {
        update_index[a].cc_array = NULL;
        update_index[a].new_nodes = NULL;
        update_index[a].size = 0;
        update_index[a].n_size = 0;
        update_index[a].state = 'o';//old
        for(j = 0; j <= THREAD_POOL_SIZE; j++) update_index[a].visited[j] = DEFAULT;

        a++;
    }

    cc->updateIndex = update_index;

}

//kathe fora pou prostithetai mia akmi ananewnei to Update Index
void refreshUpdateIndex(CC *cc, uint32_t N1, uint32_t N2) {

    uint32_t cc1, cc2;
    int found = 0;
    uint32_t new_cc = 0;
    uint32_t i = 0, realloc_size = 0, realloc_update_index_size = cc->u_size, k = 0, l = 0, j = 0;
    uint32_t *temp = NULL;


    //an kapoio ap ta N1 N2 den uparxei tote kanto DEFAULT
    //se autin tin perwptwsi koitame an einai megalitera ap ton pinaka tou CC
    if(N1 >= cc->cc_size) cc1 = DEFAULT;
    else cc1 = cc->cc_index[N1].id;

    if(N2 >= cc->cc_size) cc2 = DEFAULT;
    else cc2 = cc->cc_index[N2].id;

    //an den uparxoun tha exoun parei hdh thn timh DEFAULT

    if((cc1 != DEFAULT) && (cc1 == cc2)) return;// an einai sto idio cc tote den exei na kanei kati kai epistrefei

    if((cc1 == DEFAULT) && (cc2 == DEFAULT)) {                          // an einai kainouria cc
        i = 0;
        //vriskoume tin prwti thesi meta ta palia CC ( o -> old )
        while((cc->updateIndex[i].state == 'o') && (i < cc->u_size)) i++;

        //an xwraei
        if(i < cc->u_size) {

            l = i;
            //psaxnei na vrei tin prwti keni thesi pou tha mporouse na to valei
            while((cc->updateIndex[l].state != 'e') && (l < cc->u_size)) l++;// ( e - > empty )
            //psaxnei an exei hdh mpei se kapoia endiamesi
            while(i < l){
                if(cc->updateIndex[i].new_nodes != NULL){
                    for(k = 0; k < cc->updateIndex[i].n_size; k++){
                        if(cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                        if(cc->updateIndex[i].new_nodes[k] == N1){
                            cc1 = i;
                            found = 1; //an exei prostethei hdh krata tin thesi
                        }
                        if(cc->updateIndex[i].new_nodes[k] == N2){
                            cc2 = i;
                            found = 1; //an exei prostethei hdh krata tin thesi
                        }
                    }
                }

                i++;
            }
            //an den vrethike tote ftiakse kainourio CC
            if(!found){
                if(l < cc->u_size){
                    new_cc = l;
                }
                else{  //an de xwraei kane realloc
                    new_cc = cc->u_size;
                    realloc_update_index_size = 2 * cc->u_size;
                    cc->updateIndex = realloc(cc->updateIndex, realloc_update_index_size * sizeof(u_node));

                    for (i = (new_cc + 1); i < realloc_update_index_size; i++) {
                        cc->updateIndex[i].cc_array = NULL;
                        cc->updateIndex[i].size = 0;
                        cc->updateIndex[i].state = 'e';
                        cc->updateIndex[i].new_nodes = NULL;
                        cc->updateIndex[i].n_size = 0;
                        for(j = 0; j <= THREAD_POOL_SIZE; j++) cc->updateIndex[i].visited[j] = DEFAULT;

                    }
                    cc->u_size = realloc_update_index_size;
                }
                //kai eisagei tous komvous sto neo CC
                cc->updateIndex[new_cc].state = 'n'; // ( n - > new )
                cc->updateIndex[new_cc].n_size = INIT_NEWNODE_SIZE;
                cc->updateIndex[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
                cc->updateIndex[new_cc].new_nodes[0] = N1;
                cc->updateIndex[new_cc].new_nodes[1] = N2;
                for(j = 0; j <= THREAD_POOL_SIZE; j++) cc->updateIndex[new_cc].visited[j] = DEFAULT;
                for(k = 2; k < INIT_NEWNODE_SIZE; k++) cc->updateIndex[new_cc].new_nodes[k] = DEFAULT;

                return;

            }
            else{
                if(cc1 == cc2) return;// an to vrike kai einai hdh sto idio CC epistrefei
            }
        }
        else{
            realloc_size = 2 * cc->u_size;
            cc->updateIndex = realloc(cc->updateIndex,realloc_size * sizeof(u_node));
            new_cc = cc->u_size;
            cc->updateIndex[new_cc].state = 'n'; // ( n - > new )
            cc->updateIndex[new_cc].n_size = INIT_NEWNODE_SIZE;
            cc->updateIndex[new_cc].new_nodes = malloc(INIT_NEWNODE_SIZE * sizeof(uint32_t));
            cc->updateIndex[new_cc].new_nodes[0] = N1;
            cc->updateIndex[new_cc].new_nodes[1] = N2;
            for(j = 0; j <= THREAD_POOL_SIZE; j++) cc->updateIndex[new_cc].visited[j] = DEFAULT;

            for(k = 2; k < INIT_NEWNODE_SIZE; k++) cc->updateIndex[new_cc].new_nodes[k] = DEFAULT;
            for(l = new_cc + 1; l < realloc_size; l++){
                cc->updateIndex[l].new_nodes = NULL;
                cc->updateIndex[l].state = 'e';
                cc->updateIndex[l].n_size = 0;
                cc->updateIndex[l].cc_array = NULL;
                cc->updateIndex[l].size = 0;
                for(j = 0; j <= THREAD_POOL_SIZE; j++) cc->updateIndex[l].visited[j] = DEFAULT;
            }
            cc->u_size = realloc_size;
            return;
        }

    }
    //an kapoio ap ta 2 den uphrxe tote prepei na to valei sto CC tou allou
    //h diadikasia einai idia psaxnei na vrei an exei prostethei san neo CC apo proigoumeni RefreshUpdateIndex
    //an nai tote ta sindeei
    //an oxi to vazei sto CC tou allou
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
        //an den exei prostethei
        if(!found) {
            //kai to CC tou allou den exei akoma pinaka me nea nodes pou tha prostethoun
            //to vazei
            //alliws vriskei tin prwti keni thesi ston pinaka auton kai ton vazei ekei
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
        while((cc->updateIndex[i].state == 'o' ) && ( i < cc->u_size)) i++;

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

    //an uparxoun kai ta 2 sto CCIndex tote prepei na ta sindesei

    j = 0;
    uint32_t cur_cc = cc1;

    while(j < 2) {

        if (cc->updateIndex[cur_cc].cc_array == NULL) {       //an den exei sindethei me alla cc (o pinakas twn cc einai kenos)
            cc->updateIndex[cur_cc].cc_array = malloc(INIT_UNODE_SIZE * sizeof(uint32_t));
            temp = cc->updateIndex[cur_cc].cc_array;
            if(j == 0) temp[0] = cc2;             //elegxos se poio apo ta 2 vriskomaste
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
            }
        }
        j++;
        cur_cc = cc2;
    }
}
//Anazitisi sto UpdateIndex
int searchUpdateIndex(CC cc, uint32_t N1, uint32_t N2, uint32_t version, int thread_id) {

    uint32_t cc1 = 0;
    uint32_t cc2 = 0;
    uint32_t v = 0;
    uint32_t i = 0, j = 0;
    Stack_t *stack = createStack();
    //stack.last = NULL;

    if(N1 >= cc.cc_size) cc1 = DEFAULT;
    else cc1 = cc.cc_index[N1].id;
    if(N2 >= cc.cc_size) cc2 = DEFAULT;
    else cc2 = cc.cc_index[N2].id;

    //An kapoio ap ta 2(h kai ta 2) den uparxei sto CCIndex psaxnei mono sto new_nodes
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
    //an de vrei kapoio ap ta 2 tote sigoura de sindeontai
    if(cc1 == DEFAULT || cc2 == DEFAULT) {
        deletestack(stack);
        return NOT_FOUND;
    }
    //an den einai sto idio kainourio CC prepei na psaksei an sindeontai
    // opote koitaei stous geitones tou kai poious geitones exoun autoi klp...
    // mexri na vrei to cc2
    if(cc1 != cc2) {
        pushinstack(stack, cc1);
        while(!stackisempty(stack)) {
            v = popfromstack(stack);
            if(v != cc2) {
                //if (search(explored, v, HT_BIG, version) == NOT_FOUND) {
                if (cc.updateIndex[v].visited[thread_id] != version) {
                    //insert(explored, v, HT_BIG, version);
                    cc.updateIndex[v].visited[thread_id] = version;

                    if(cc.updateIndex[v].cc_array == NULL) {
                        continue;
                    }

                    i = 0;

                    while((cc.updateIndex[v].cc_array[i] != DEFAULT ) && ( i < cc.updateIndex[v].size)) {
                        pushinstack(stack, cc.updateIndex[v].cc_array[i]);
                        i++;
                    }
                }
            }
            else {
                deletestack(stack);
                return FOUND;
            }
        }
        deletestack(stack);
        return NOT_FOUND;
    }
    else{
        deletestack(stack);
        return FOUND;//an cc1 == cc2
    }
}

uint32_t updateCCIndex(CC *cc, uint32_t version, uint32_t new_size, int thread_id) {

    uint32_t v = 0, w = 0;
    uint32_t i = 0, j = 0;
    Stack_t *stack = createStack();
    uint32_t k;
    uint32_t parent_cc = 0;
    //uint32_t new_cc[new_size];
    uint32_t  *new_cc = malloc(new_size*sizeof(uint32_t));
    uint32_t old_cc = 0;

    //arxikopoisi tou pinaka gia tis allages sta CC
    for(i = 0; i < new_size; i++) new_cc[i] = DEFAULT;

  //  stack.last = NULL;

    // an de xwraei ta kanouria CC kane realloc
    if(new_size > cc->cc_size){
        cc->cc_index = realloc(cc->cc_index, new_size*sizeof(uint32_t));
        for(j = cc->cc_size; j < new_size; j++){
            cc->cc_index[j].id = DEFAULT;
        }
    }

    //gia tous neous komvous pou den upirxan prin sto CCIndex
    for(i = 0; i < cc->u_size; i++ ) {
        if(cc->updateIndex[i].new_nodes != NULL) {
            for(k = 0 ; k < cc->updateIndex[i].n_size ; k++) {
                if(cc->updateIndex[i].new_nodes[k] == DEFAULT) break;
                cc->cc_index[cc->updateIndex[i].new_nodes[k]].id = i;
            }
        }
    }
    parent_cc = 0;//gia na bgoun me ti seira ta CC ap to 0

    //vres gia kathe CC poio tha einai to kainourio pou tha tou antistoixei
    for(i = 0 ; i < cc->u_size ; i++) {
        if(cc->updateIndex[i].state == 'e') break;
        //if(search(explored, i, HT_BIG, version) == FOUND) continue;
        if(cc->updateIndex[i].visited[thread_id] == version) continue;
        pushinstack(stack, i);
        new_cc[i] = parent_cc;
        //insert(explored, i, HT_BIG, version);
        cc->updateIndex[i].visited[thread_id] = version;
        while(!stackisempty(stack)) {
            v = popfromstack(stack);

            if(cc->updateIndex[v].cc_array != NULL) {
                for (k = 0; k < cc->updateIndex[v].size; k++) {
                    w = cc->updateIndex[v].cc_array[k];
                    if(w == DEFAULT) break;
                    if(new_cc[w] != DEFAULT) continue;
                    pushinstack(stack, w);
                    new_cc[w] = parent_cc;
                    //insert(explored, w, HT_BIG, version);
                    cc->updateIndex[w].visited[thread_id] = version;
                }
            }

        }
        parent_cc++;
    }
    //kai meta allakse ta
    for(k = 0 ; k < cc->cc_size ; k++) {
        old_cc = cc->cc_index[k].id;
        if(old_cc == DEFAULT) continue;
        cc->cc_index[k].id = new_cc[old_cc];
    }
    deletestack(stack);

    //diagrafi tou paliou updateIndex
    for(i = 0; i < cc->u_size; i++){
        free(cc->updateIndex[i].cc_array);
        free(cc->updateIndex[i].new_nodes);

        cc->updateIndex[i].cc_array = NULL;
        cc->updateIndex[i].new_nodes = NULL;
    }
    free(cc->updateIndex);
    cc->updateIndex = NULL;

    cc->u_size = parent_cc;//to megethos tou epomenou UpdateIndex tha einai iso me to teleutaio CC pou mpike

    initUpdateIndex(cc);
    free(new_cc);

    return parent_cc;
}
//free CC Index
void destroyCCIndex(CC *cc){

    uint32_t i = 0;

    for(i = 0; i < cc->u_size; i++){
        free(cc->updateIndex[i].cc_array);
        free(cc->updateIndex[i].new_nodes);
    }
    free(cc->updateIndex);
    free(cc->cc_index);
    free(cc);
}