#include "scc.h"

uint32_t peek(Stack *stack) {

    if(stack == NULL) return DEFAULT;
    if(stack->last == NULL) return DEFAULT;

    return stack->last->id;

}

SCC* estimateStronglyConnectedComponents(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes, ht_Node *explored, ht_Node *explored2, ht_Node* explored3, uint32_t version) {


    SCC *scc = tarjan(index_out, buffer_out, size_out, num_nodes, explored, explored2, explored3, version);

    return scc;
}

SCC* tarjan(ind *index_out, list_node *buffer_out, uint32_t size_out, uint32_t num_nodes, ht_Node* explored, ht_Node* explored_twice, ht_Node* explored_scc, uint32_t version) {

    Stack scc_stack;
    Stack dfs_stack;

    Stack parent_stack;
    Stack next_child;

    list_node *neighbors_out;
    ptrdiff_t offset_out;
    uint32_t size = size_out, index = 0, realloc_node_size;
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
    scc_counter = 1;

    for(i = 0 ; i < size ; i++) {
        if(lookup(index_out, i, size_out) == NOT_EXIST) continue;
        if(search(explored_scc, i, HT_BIG, version) == FOUND) continue;


        push(&parent_stack, i);
        push(&scc_stack, i);

        while(!stackIsEmpty(&scc_stack)){

            v = pop(&next_child);
            if(v == DEFAULT) v = peek(&parent_stack);

            if(search(explored, v, HT_BIG, version) == NOT_FOUND) {
                index_out[v].index = index;
                index_out[v].lowlink = index;
                //index_out[v].all_children_in_scc = 0;
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
                    if(k >= N) break;
                    w = neighbors_out->neighbor[k];
                    if(w == DEFAULT) break;

                    /////////////////////////////////////////////////////////////
                    if(search(explored_scc, w, HT_BIG, version) == FOUND){

                        index_out[v].all_children_in_scc++;//de doulevei gia to tarjan.txt giati den metrame pote ta paidia toy 8 giati einai ola sto explored_scc
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
                        //if(search(explored_twice, w, HT_BIG, version) == NOT_FOUND) {
                            if (index_out[v].lowlink > index_out[w].lowlink)
                                index_out[v].lowlink = index_out[w].lowlink;
                       // }
                        /*else{
                            if(index_out[v].lowlink > index_out[w].index) index_out[v].lowlink = index_out[w].index;
                        }*/
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
                        for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                        a = 0;
                        printf("SCC: %d\n",scc_counter);
                        do{
                            w = pop(&scc_stack);
                            insert(explored_scc, w, HT_BIG, version);

                            scc->components[scc_counter].included_node_ids[a] = w;

                            if(a == (scc->components[scc_counter].node_array_size-1)) {
                                realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                                scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
                                for(r = scc->components[scc_counter].node_array_size ; r < realloc_node_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                                scc->components[scc_counter].node_array_size = realloc_node_size;
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
                if(search(explored, v, HT_BIG, version) == NOT_FOUND) {
                    insert(explored, v, HT_BIG, version);
                    insert(explored_scc, v, HT_BIG, version);

                    scc->components[scc_counter].component_id = scc_counter;
                    scc->components[scc_counter].included_nodes_count = 0;
                    scc->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                    scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
                    for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                    a = 0;

                    scc->components[scc_counter].included_node_ids[a] = w;

                    if(a == (scc->components[scc_counter].node_array_size-1)) {
                        realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                        scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
                        for(r = scc->components[scc_counter].node_array_size ; r < realloc_node_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                        scc->components[scc_counter].node_array_size = realloc_node_size;
                    }
                    scc->components[scc_counter].included_nodes_count++;
                    scc->id_belongs_to_component[w] = scc_counter;

                    a++;
                    printf("SOLO SCC: %d\n",scc_counter);
                    printf("%d\n", w);

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

    }
























/*


    scc_stack.last = NULL;
    dfs_stack.last = NULL;

    index = 1;

    for(i = 0 ; i < size ; i++) {
        if(lookup(index_out, i, size_out) == NOT_EXIST) continue;
        if(search(explored_scc, i, HT_BIG, version) == FOUND) continue;


        push(&dfs_stack, i);
        push(&scc_stack, i);

        while(!stackIsEmpty(&scc_stack)){

            v = peek(&dfs_stack);
            if(search(explored, v, HT_BIG, version) == NOT_FOUND) {
                index_out[v].index = index;
                index_out[v].lowlink = index;
                index_out[v].all_children_in_scc = 0;
                index++;
                insert(explored, v, HT_BIG, version);
            }

            offset_out = getListHead(index_out, v);
            neighbors_out = buffer_out + offset_out;

            if(index_out[v].num_of_children != 0 && index_out[v].all_children_in_scc != index_out[v].num_of_children){   //if v has children

                if(search(explored_twice, v, HT_BIG, version) == FOUND) {
                        pop(&dfs_stack);
                }
                k = 0;
                while (1) {
                    if(k >= N) break;
                    w = neighbors_out->neighbor[k];
                    if(w == DEFAULT) break;

                    //index_out[v].all_children_in_scc = 0;

                    if(search(explored_scc, w, HT_BIG, version) == FOUND){

                        index_out[v].all_children_in_scc++;//de doulevei gia to tarjan.txt giati den metrame pote ta paidia toy 8 giati einai ola sto explored_scc
                        k++;
                        if(k >= N && neighbors_out->nextListNode != -1) {
                            neighbors_out = buffer_out + neighbors_out->nextListNode;
                            k = 0;
                        }
                        //else break;
                        continue;
                    }
                   // index_out[v].all_children_in_scc = 0;
                    if(search(explored, w, HT_BIG, version) == NOT_FOUND){
                        index_out[w].index = index;
                        index_out[w].lowlink = index;
                        index++;
                        push(&scc_stack, w);
                        push(&dfs_stack, w);
                        insert(explored, w, HT_BIG, version);

                        //num_of_children[v]++;
                        index_out[w].all_children_in_scc = 0;
                    }
                    else{
                        if(search(explored_twice, w, HT_BIG, version) == NOT_FOUND){

                            if (index_out[v].lowlink > index_out[w].lowlink)
                                index_out[v].lowlink = index_out[w].lowlink;

                            push(&dfs_stack, w);

                            insert(explored_twice, w, HT_BIG, version);
                        }
                        else if(peek(&dfs_stack) == v){

                            if(index_out[v].lowlink > index_out[w].index) index_out[v].lowlink = index_out[w].index;

                            scc->components[scc_counter].component_id = scc_counter;
                            scc->components[scc_counter].included_nodes_count = 0;
                            scc->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                            scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
                            for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                            a = 0;


                            v = pop(&dfs_stack);
                            while(index_out[v].index != index_out[v].lowlink) v = pop(&dfs_stack);
                            while(1){
                                w = pop(&scc_stack);
                                if(w == DEFAULT) break;
                                insert(explored_scc, w, HT_BIG, version);

                                scc->components[scc_counter].included_node_ids[a] = w;

                                if(a == (scc->components[scc_counter].node_array_size-1)) {
                                    realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                                    scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
                                    for(r = scc->components[scc_counter].node_array_size ; r < realloc_node_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                                    scc->components[scc_counter].node_array_size = realloc_node_size;
                                }
                                scc->components[scc_counter].included_nodes_count++;
                                scc->id_belongs_to_component[w] = scc_counter;

                                a++;
                                printf("ta scc %d exei ta %d ids\n", scc_counter, w);
                                if(w == v) break;
                            }
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
                        else push(&dfs_stack, v);
                    }
                    k++;
                    if(k >= N && neighbors_out->nextListNode != -1) {
                        neighbors_out = buffer_out + neighbors_out->nextListNode;
                        k = 0;
                    }
                }

            }
            else{
                w = pop(&scc_stack);
                pop(&dfs_stack);

                insert(explored, w, HT_BIG, version);
                insert(explored_scc, w, HT_BIG, version);

                scc->components[scc_counter].component_id = scc_counter;
                scc->components[scc_counter].included_nodes_count = 0;
                scc->components[scc_counter].node_array_size = NODE_IDS_SIZE;
                scc->components[scc_counter].included_node_ids = malloc(scc->components[scc_counter].node_array_size*sizeof(uint32_t));
                for(r = 0 ; r < scc->components[scc_counter].node_array_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                a = 0;

                scc->components[scc_counter].included_node_ids[a] = w;

                if(a == (scc->components[scc_counter].node_array_size-1)) {
                    realloc_node_size = 2*scc->components[scc_counter].node_array_size;
                    scc->components[scc_counter].included_node_ids = realloc(scc->components[scc_counter].included_node_ids, realloc_node_size*sizeof(uint32_t));
                    for(r = scc->components[scc_counter].node_array_size ; r < realloc_node_size ; r++) scc->components[scc_counter].included_node_ids[r] = DEFAULT;
                    scc->components[scc_counter].node_array_size = realloc_node_size;
                }
                scc->components[scc_counter].included_nodes_count++;
                scc->id_belongs_to_component[w] = scc_counter;

                a++;
                printf("SOLO: ta scc %d exei ta %d ids\n", scc_counter, w);
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
    }*/

    scc->components_count = scc_counter;

    deleteStack(&scc_stack);
    deleteStack(&dfs_stack);

    return scc;
}

void destroyStronglyConnectedComponents(SCC* scc){

    uint32_t i = 0;

    for(i = 0; i < scc->components_count; i++){
        free(scc->components[i].included_node_ids);
    }
    free(scc->id_belongs_to_component);
    free(scc->components);
    free(scc);

}