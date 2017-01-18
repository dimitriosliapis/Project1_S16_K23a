#include "scc.h"

int peek(Stack *stack) {

    if(stack == NULL) return DEFAULT;
    if(stack->last == NULL) return DEFAULT;

    return stack->last->id;

}


/*SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, int size_out, int num_nodes, ht_Node *explored, ht_Node *explored2, ht_Node* explored3, int version) {


    SCC *scc = tarjan(index_out, buffer_out, size_out, num_nodes, explored, explored2, explored3, version);

    return scc;
}*/

//dimiourgia SCC
SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, int num_nodes, int version, int thread_id) {

    Stack scc_stack;
    int index = 0;
    int i = 0, r = 0;


    SCC *scc = malloc(sizeof(SCC));

    scc->components = malloc(COMPONENT_SIZE*sizeof(Component));
    scc->component_size = COMPONENT_SIZE;
    for(r = 0; r < scc->component_size; r++){
        scc->components[r].included_node_ids = NULL;
        scc->components[r].included_nodes_count = 0;
        scc->components[r].node_array_size = 0;
    }
    scc->components_count = 0;

    scc->id_belongs_to_component = malloc(num_nodes*sizeof(int));
    for(i = 0; i < num_nodes; i++){
        scc->id_belongs_to_component[i] = DEFAULT;
    }

    scc_stack.last = NULL;

    index = 1;

    //gia kathe komvo tou grafou an den einai visited kalei ton tarjan
    for(i = 0; i < num_nodes; i++) {
        if(lookup(index_out, i, num_nodes) == NOT_EXIST) continue;
        //if(search(explored, i, HT_BIG, version) == FOUND) continue;
        if(index_out[i].visited[thread_id] == version) continue;
        scc = tarjanRecursive(&scc, index_out, buffer_out, num_nodes, version, i, &index, &scc_stack, thread_id);
    }
    return scc;
}


SCC* tarjanRecursive(SCC **scc, ind *index_out, list_node *buffer_out, int num_nodes, int version, int v, int *index, Stack *scc_stack, int thread_id){

    int k = 0, w = 0, a = 0, r = 0, realloc_node_size;
    ptrdiff_t offset_out;
    list_node *neighbors_out;
    int scc_counter = (*scc)->components_count;

    //insert(explored, v, HT_BIG, version);
    index_out[v].visited[thread_id] = version;

    //an den xwraei to kainourio SCC kanei realloc
    if(scc_counter == (*scc)->component_size){
        (*scc)->component_size *= 2;
        (*scc)->components = realloc((*scc)->components, (*scc)->component_size * sizeof(Component));
        for(r = scc_counter; r < (*scc)->component_size; r++){
            (*scc)->components[r].included_node_ids = NULL;
            (*scc)->components[r].included_nodes_count = 0;
            (*scc)->components[r].node_array_size = 0;
        }
    }

    //o komvos pou episkeftetai auti tin stigmi
    index_out[v].index = *index;
    index_out[v].lowlink = *index;
    (*index)++;
    push(scc_stack, v);
    index_out[v].onStack = 1;//einai sto scc stack

    //an exei paidia
    if(index_out[v].num_of_children != 0) {

        offset_out = getListHead(index_out, v);
        neighbors_out = buffer_out + offset_out;

        while(k < N){

            w = neighbors_out->neighbor[k];

            //an den exei alla paidia
            if(w == DEFAULT) break;
            //if(search(explored, w, HT_BIG, version) == NOT_FOUND){

            //an to paidi tou den einai visited
            if(index_out[w].visited[thread_id] != version) {
                *scc = tarjanRecursive(scc, index_out, buffer_out, num_nodes, version, w, index, scc_stack, thread_id);
                if (index_out[v].lowlink > index_out[w].lowlink)
                    index_out[v].lowlink = index_out[w].lowlink;
            }
            else if(index_out[w].onStack){ // alliws an einai sto scc stack
                if(index_out[v].lowlink > index_out[w].index)
                    index_out[v].lowlink = index_out[w].index;
            }

            k++;
            if(k >= N && neighbors_out->nextListNode != -1) {
                neighbors_out = buffer_out + neighbors_out->nextListNode;
                k = 0;
            }
        }
    }
    if(index_out[v].lowlink == index_out[v].index) {
        scc_counter = (*scc)->components_count;
        (*scc)->components[scc_counter].included_nodes_count = 0;
        (*scc)->components[scc_counter].node_array_size = NODE_IDS_SIZE;
        (*scc)->components[scc_counter].included_node_ids = malloc((*scc)->components[scc_counter].node_array_size * sizeof(int));
        (*scc)->components[scc_counter].included_node_ids[0] = DEFAULT;

        a = 0;
        //ftiakse to SCC
        do {
            w = pop(scc_stack);
            index_out[w].onStack = 0;

            (*scc)->components[scc_counter].included_node_ids[a] = w;

            if (a == ((*scc)->components[scc_counter].node_array_size - 1)) {
                realloc_node_size = 2 * (*scc)->components[scc_counter].node_array_size;
                (*scc)->components[scc_counter].included_node_ids = realloc((*scc)->components[scc_counter].included_node_ids,realloc_node_size * sizeof(int));
                (*scc)->components[scc_counter].node_array_size = realloc_node_size;
            }
            (*scc)->components[scc_counter].included_node_ids[a + 1] = DEFAULT;
            (*scc)->components[scc_counter].included_nodes_count++;
            (*scc)->id_belongs_to_component[w] = scc_counter;

            a++;


        } while (w != v);
        (*scc)->components_count++;//gia to epomeno SCC
    }
    return *scc;

}


// epanaliptikos tarjan
/*
SCC* tarjan(ind *index_out, list_node *buffer_out, int size_out, int num_nodes, ht_Node* explored, ht_Node* explored_twice, ht_Node* explored_scc, int version) {


    //Stack_t* scc_stack = createStack();
    //Stack_t* parent_stack = createStack();
    //Stack_t* next_child = createStack();

    //Stack dfs_stack;

    Stack scc_stack;
    Stack parent_stack;
    Stack next_child;

    list_node *neighbors_out;
    ptrdiff_t offset_out;
    int size = 0, index = 0, realloc_node_size;
    int i = 0, v = 0, k = 0, a = 0, w = 0, r = 0;
    int scc_counter = 0;
    //unsigned int num_of_children[num_nodes];

    //for(r = 0; r < num_nodes; r++) num_of_children[r] = 0;

    SCC *scc = malloc(sizeof(SCC));

    scc->components = malloc(COMPONENT_SIZE*sizeof(Component));
    scc->component_size = COMPONENT_SIZE;
    for(r = 0; r < scc->component_size; r++){
        scc->components[r].included_node_ids = NULL;
        scc->components[r].component_id = DEFAULT;
        scc->components[r].included_nodes_count = 0;
        scc->components[r].node_array_size = 0;
    }
    //for(k = 0 ; k < COMPONENT_SIZE ; k++) scc->components[k] = NULL;
    scc->components_count = 0;

    scc->id_belongs_to_component = malloc(num_nodes*sizeof(int));
    for(i = 0; i < num_nodes; i++){
        scc->id_belongs_to_component[i] = DEFAULT;
    }

    scc_stack.last = NULL;
    next_child.last = NULL;
    parent_stack.last = NULL;

    index = 1;
    scc_counter = 0;
    size = num_nodes;

    for(i = 0 ; i < size ; i++) {
        if(lookup(index_out, i, size_out) == NOT_EXIST) continue;
        if(search(explored_scc, i, HT_BIG, version) == FOUND) continue;
        //if(search(explored, i, HT_BIG, version) == FOUND) continue;
//        printf("%d\n",i);


        push(&parent_stack, i);
        push(&scc_stack, i);

        index_out[i].index = index;
        index_out[i].lowlink = index;
        index_out[v].all_children_in_scc = 0;
        index++;
        insert(explored, i, HT_BIG, version);

        while(!stackIsEmpty(&scc_stack)){

            v = pop(&next_child);
            if(v == DEFAULT) v = peek(&parent_stack);

            if(search(explored, v, HT_BIG, version) == NOT_FOUND) {
                index_out[v].index = index;
                index_out[v].lowlink = index;
                index_out[v].all_children_in_scc = 0;
                index++;
            }

            if(index_out[v].num_of_children != 0 && index_out[v].all_children_in_scc != index_out[v].num_of_children){

                if(search(explored, v, HT_BIG, version) == NOT_FOUND) {
                    push(&parent_stack, v);
                    insert(explored, v, HT_BIG, version);
                }

                offset_out = getListHead(index_out, v);
                neighbors_out = buffer_out + offset_out;

                k = 0;

                while (1) {
                    if(k >= N) {
                        w = DEFAULT;
                        break;
                    }
                    w = neighbors_out->neighbor[k];
                    if(w == DEFAULT) {
                        insert(explored_twice, v, HT_BIG, version);
                        break;
                    }

                    /////////////////////////////////////////////////////////////
                    if(search(explored_scc, w, HT_BIG, version) == FOUND){

                        index_out[v].all_children_in_scc++;
                        k++;
                        if(k >= N && neighbors_out->nextListNode != -1) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            k = 0;
                        }
                        //else break;
                        continue;
                    }
                    /////////////////////////////////////////////////////////////

                    if(search(explored, w, HT_BIG, version) == NOT_FOUND){
                        push(&next_child, w);
                        push(&scc_stack, w);
                        break;
                    }
                    else{
                       // if(search(explored_twice, w, HT_BIG, version) == NOT_FOUND) {
                            if (index_out[v].lowlink > index_out[w].lowlink)
                                index_out[v].lowlink = index_out[w].lowlink;
                        //}
                        if(search(explored_twice, w, HT_BIG, version) == FOUND){
                            if(index_out[v].lowlink > index_out[w].index) index_out[v].lowlink = index_out[w].index;
                           // push(&next_child, w);
                            //insert(explored_twice, w, HT_BIG, version);
                        }
                    }


                    k++;
                    if(k >= N && neighbors_out->nextListNode != -1) {
                        neighbors_out = buffer_out + neighbors_out->nextListNode;
                        k = 0;
                    }
                }
                if(w == DEFAULT){
                    pop(&parent_stack);
                    if(index_out[v].lowlink == index_out[v].index){
                        scc->components[scc_counter].component_id = scc_counter;
                        scc->components[scc_counter].included_nodes_count = 0;
                        scc->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                        scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(int));
                        //for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                        scc->components[scc_counter].included_node_ids[0] = DEFAULT;
                        a = 0;
                        printf("SCC: %d\n",scc_counter);
                        do{
                            w = pop(&scc_stack);
                            insert(explored_scc, w, HT_BIG, version);

                            scc->components[scc_counter].included_node_ids[a] = w;

                            if(a == (scc->components[scc_counter].node_array_size-1)) {
                                realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                                scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(int));
                               // for(r = scc->components[scc_counter].node_array_size ; r < realloc_node_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                                scc->components[scc_counter].included_node_ids[a] = DEFAULT;
                                scc->components[scc_counter].node_array_size = realloc_node_size;
                            }
                            else{
                                scc->components[scc_counter].included_node_ids[a+1] = DEFAULT;
                            }
                            scc->components[scc_counter].included_nodes_count++;
                            scc->id_belongs_to_component[w] = scc_counter;

                            a++;


                            printf("%d\n", w);
                        }while(w != v);
                        scc_counter++;

                        if(scc_counter == scc->component_size){
                            scc->component_size *= 2;
                            scc->components = realloc(scc->components, scc->component_size * sizeof(Component));
                            for(r = scc_counter; r < scc->component_size; r++){
                                scc->components[r].included_node_ids = NULL;
                                scc->components[r].component_id = DEFAULT;
                                scc->components[r].included_nodes_count = 0;
                                scc->components[r].node_array_size = 0;
                            }
                        }
                    }
                }
            }
            else{
                if(peek(&scc_stack) == v) pop(&scc_stack);
                if(peek(&parent_stack) == v) pop(&parent_stack);
                if(search(explored_scc, v, HT_BIG, version) == NOT_FOUND) {
                    //insert(explored, v, HT_BIG, version);
                    insert(explored_scc, v, HT_BIG, version);



                    scc->components[scc_counter].component_id = scc_counter;
                    scc->components[scc_counter].included_nodes_count = 0;
                    scc->components[scc_counter].node_array_size = 1;
                    scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(int));
                    //for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                    scc->components[scc_counter].included_node_ids[0] = DEFAULT;
                    a = 0;

                    scc->components[scc_counter].included_node_ids[a] = v;

                   */
/* if(a == (scc->components[scc_counter].node_array_size-1)) {
                        realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                        scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(int));
                        for(r = scc->components[scc_counter].node_array_size ; r < realloc_node_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                        scc->components[scc_counter].node_array_size = realloc_node_size;
                    }*//*

                    scc->components[scc_counter].included_nodes_count++;
                    scc->id_belongs_to_component[v] = scc_counter;

                    //a++;
                   // printf("SOLO SCC: %d\n",scc_counter);
                   // printf("%d\n", v);

                    scc_counter++;
                    if(scc_counter == scc->component_size){
                        scc->component_size *= 2;
                        scc->components = realloc(scc->components, scc->component_size * sizeof(Component));
                        for(r = scc_counter; r < scc->component_size; r++){
                            scc->components[r].included_node_ids = NULL;
                            scc->components[r].component_id = DEFAULT;
                            scc->components[r].included_nodes_count = 0;
                            scc->components[r].node_array_size = 0;
                        }
                    }

                }
                else  printf("EXPLORED\n");

            }
        }

    }
    scc->components_count = scc_counter;

    deleteStack(&scc_stack);
    deleteStack(&next_child);
    deleteStack(&parent_stack);

    return scc;
}
*/

int scc_bBFS(ind *index_in,
             ind *index_out,
             list_node *buffer_in,
             list_node *buffer_out, SCC *scc, int start,
             int end,
             Queue *frontierF,
             Queue *frontierB,
             int version,
             int thread_id) {

    list_node *neighbors = NULL;
    int node = DEFAULT, successor = DEFAULT, childrenF = 0, childrenB = 0;
    int i = 0, steps = 0, curr_steps = 0, min_steps = -1, path = 0, scc_num;
    ptrdiff_t offset = 0;
    int counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, stepsF = 0, stepsB = 0;

    if (start == end)   // an o komvos ekkinhshs einai o komvos stoxos tote steps=0
        return 0;

    scc_num = scc->id_belongs_to_component[start];

    index_out[start].visited[thread_id] = version;
    enq(frontierF, start);
    childrenF = index_out[start].num_of_children;
    counterF++;

    index_in[end].visited[thread_id] = version;
    enq(frontierB, end);
    childrenB = index_in[end].num_of_children;
    counterB++;

    while (!isEmpty(frontierF) && !isEmpty(frontierB)) {    // oso ta 2 synora den einai adeia

        if (childrenF <= childrenB) {   // epilogh bfs analoga me ton arithmo ton komvwn pros epektash

            stepsF++;
            while (counterF != 0) {     // epanalhpsh gia olous tous komvous tou epipedou

                node = deq(frontierF);  // exagwgh tou prwtou komvou apo to synoro

                offset = getListHead(index_out, node);
                if (offset != -1) {     // elegxos gia ton an exei geitones

                    neighbors = buffer_out + offset;
                    while (i < N) {     // gia kathe geitona

                        successor = neighbors->neighbor[i];
                        if (successor != DEFAULT && scc->id_belongs_to_component[successor] == scc_num) {
                            if (index_out[successor].visited[thread_id] != version) {      // an den ton exei epispeftei o idios
                                index_out[successor].visited[thread_id] = version;         // ton episkeptetai

                                if (index_in[successor].visited[thread_id] == version) {   // goal afou ton exei episkeptei o allos
                                    restartQueue(frontierF);
                                    restartQueue(frontierB);
                                    return stepsB + stepsF;

                                } else {   // alliws eisagetai sto synoro
                                    enq(frontierF, successor);
                                    counterFS++;
                                    childrenF += index_out[successor].num_of_children;
                                }

                            }

                        } else
                            break;

                        i++;

                        // an exei ki allous geitones se epomeno listnode synexizei
                        if (i == N && neighbors->nextListNode != -1) {
                            neighbors = buffer_out + neighbors->nextListNode;
                            i = 0;
                        }
                    }

                    i = 0;
                }

                counterF--;
            }

            counterF = counterFS;
            counterFS = 0;
        }

        if (childrenB < childrenF) {    // epilogh bfs analoga me ton arithmo ton komvwn pros epektash

            stepsB++;
            while (counterB != 0) {

                node = deq(frontierB);  // exagwgh tou prwtou komvou apo to synoro

                offset = getListHead(index_in, node);
                if (offset != -1) {     // elegxos gia ton an exei geitones

                    neighbors = buffer_in + offset;
                    while (i < N) {     // gia kathe geitona

                        successor = neighbors->neighbor[i];
                        if (successor != DEFAULT && scc->id_belongs_to_component[successor] == scc_num) {

                            if (index_in[successor].visited[thread_id] != version) {       // an den ton exei episkeptei o idios
                                index_in[successor].visited[thread_id] = version;          // ton episkeptetai

                                if (index_out[successor].visited[thread_id] == version) {  // goal afou ton exei episkeptei o allos

                                    restartQueue(frontierB);
                                    restartQueue(frontierF);
                                    return stepsF + stepsB;

                                } else {    // alliws eisagetai sto synoro
                                    enq(frontierB, successor);
                                    counterBS++;
                                    childrenB += index_in[successor].num_of_children;
                                }

                            }

                        } else
                            break;

                        i++;

                        // an exei ki allous geitones se epomeno listnode synexizei
                        if (i == N && neighbors->nextListNode != -1) {
                            neighbors = buffer_in + neighbors->nextListNode;
                            i = 0;
                        }
                    }

                    i = 0;
                }

                counterB--;
            }

            counterB = counterBS;
            counterBS = 0;
        }
    }

    restartQueue(frontierF);
    restartQueue(frontierB);

    return -1;  // an den vrethei monopati epistrefei -1
}

//free SCC
void destroyStronglyConnectedComponents(SCC* scc){

    int i = 0;

    for(i = 0; i < scc->components_count; i++){
        free(scc->components[i].included_node_ids);
    }
    free(scc->id_belongs_to_component);
    free(scc->components);
    free(scc);

}