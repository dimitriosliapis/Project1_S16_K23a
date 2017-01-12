#include "scc.h"

/*uint32_t peek(Stack *stack) {

    if(stack == NULL) return DEFAULT;
    if(stack->last == NULL) return DEFAULT;

    return stack->last->id;

}*/


/*SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes, ht_Node *explored, ht_Node *explored2, ht_Node* explored3, uint32_t version) {


    SCC *scc = tarjan(index_out, buffer_out, size_out, num_nodes, explored, explored2, explored3, version);

    return scc;
}*/

//dimiourgia SCC
SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, uint32_t num_nodes, uint32_t version, int thread_id) {

    Stack_t *scc_stack = createStack();
    uint32_t index = 0;
    uint32_t i = 0, r = 0;


    SCC *scc = malloc(sizeof(SCC));

    scc->components = malloc(COMPONENT_SIZE*sizeof(Component));
    scc->component_size = COMPONENT_SIZE;
    for(r = 0; r < scc->component_size; r++){
        scc->components[r].included_node_ids = NULL;
        scc->components[r].included_nodes_count = 0;
        scc->components[r].node_array_size = 0;
    }
    scc->components_count = 0;

    scc->id_belongs_to_component = malloc(num_nodes*sizeof(uint32_t));
    for(i = 0; i < num_nodes; i++){
        scc->id_belongs_to_component[i] = DEFAULT;
    }

    //scc_stack.last = NULL;

    index = 1;

    //gia kathe komvo tou grafou an den einai visited kalei ton tarjan
    for(i = 0; i < num_nodes; i++) {
        if(lookup(index_out, i, num_nodes) == NOT_EXIST) continue;
        //if(search(explored, i, HT_BIG, version) == FOUND) continue;
        if(index_out[i].visited[thread_id] == version) continue;
        scc = tarjanRecursive(&scc, index_out, buffer_out, num_nodes, version, i, &index, scc_stack, thread_id);
    }
    return scc;
}


SCC* tarjanRecursive(SCC **scc, ind *index_out, list_node *buffer_out, uint32_t num_nodes, uint32_t version, uint32_t v, uint32_t *index, Stack_t *scc_stack, int thread_id){

    uint32_t k = 0, w = 0, a = 0, r = 0, realloc_node_size;
    ptrdiff_t offset_out;
    list_node *neighbors_out;
    uint32_t scc_counter = (*scc)->components_count;

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
    pushinstack(scc_stack, v);
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
        (*scc)->components[scc_counter].included_node_ids = malloc((*scc)->components[scc_counter].node_array_size * sizeof(uint32_t));
        (*scc)->components[scc_counter].included_node_ids[0] = DEFAULT;

        a = 0;
        //ftiakse to SCC
        do {
            w = popfromstack(scc_stack);
            index_out[w].onStack = 0;

            (*scc)->components[scc_counter].included_node_ids[a] = w;

            if (a == ((*scc)->components[scc_counter].node_array_size - 1)) {
                realloc_node_size = 2 * (*scc)->components[scc_counter].node_array_size;
                (*scc)->components[scc_counter].included_node_ids = realloc((*scc)->components[scc_counter].included_node_ids,realloc_node_size * sizeof(uint32_t));
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
SCC* tarjan(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes, ht_Node* explored, ht_Node* explored_twice, ht_Node* explored_scc, uint32_t version) {


    //Stack_t* scc_stack = createStack();
    //Stack_t* parent_stack = createStack();
    //Stack_t* next_child = createStack();

    //Stack dfs_stack;

    Stack scc_stack;
    Stack parent_stack;
    Stack next_child;

    list_node *neighbors_out;
    ptrdiff_t offset_out;
    uint32_t size = 0, index = 0, realloc_node_size;
    uint32_t i = 0, v = 0, k = 0, a = 0, w = 0, r = 0;
    uint32_t scc_counter = 0;
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

    scc->id_belongs_to_component = malloc(num_nodes*sizeof(uint32_t));
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
                        scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
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
                                scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
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
                    scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
                    //for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                    scc->components[scc_counter].included_node_ids[0] = DEFAULT;
                    a = 0;

                    scc->components[scc_counter].included_node_ids[a] = v;

                   */
/* if(a == (scc->components[scc_counter].node_array_size-1)) {
                        realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                        scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
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

//free SCC
void destroyStronglyConnectedComponents(SCC* scc){

    uint32_t i = 0;

    for(i = 0; i < scc->components_count; i++){
        free(scc->components[i].included_node_ids);
    }
    free(scc->id_belongs_to_component);
    free(scc->components);
    free(scc);

}