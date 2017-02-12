#include "search.h"
#include "scc.h"
#include "grail.h"

extern SCC *scc;
extern GrailIndex *grail;

Queue *createQueue() {

    int i = 0;
    Queue *queue = NULL;
    queue = malloc(sizeof(Queue));

    if (queue == NULL)
        return NULL;
    else {
        queue->size = QUEUE_SIZE;
        queue->ids = malloc(QUEUE_SIZE * sizeof(uint32_t));
        queue->first = 0;
        queue->last = -1;
        queue->count = 0;
        return queue;
    }
}

int isEmpty(Queue *queue) {

    return (queue->count == 0);
}

int enq(Queue *queue, uint32_t id) {

    if (queue->count >= queue->size) {
        uint32_t *temp = malloc(queue->size*sizeof(uint32_t));
        int i = 0;
        for(i = 0 ; i < queue->size ; i++) {
            temp[i] = queue->ids[queue->first];
            queue->first = (queue->first + 1) % queue->size;
        }
        queue->ids = realloc(queue->ids, queue->size * 2 * sizeof(uint32_t));
        for(i = 0 ; i < queue->size ; i++) {
            queue->ids[i] = temp[i];
        }
        free(temp);
        queue->first = 0;
        queue->last = queue->size;
        queue->size *= 2;
    }

    queue->last = (queue->last + 1) % queue->size;
    queue->ids[queue->last] = id;
    queue->count++;

    return 0;
}

uint32_t deq(Queue *queue) {

    uint32_t id = DEFAULT;

    if (queue->count == 0)
        return DEFAULT;

    id = queue->ids[queue->first];
    queue->first = (queue->first + 1) % queue->size;
    queue->count--;
    return id;
}

void restartQueue(Queue *queue) {

    int i = 0;

    for (i = 0; i < queue->size; i++)
        queue->ids[i] = DEFAULT;

    queue->first = 0;
    queue->last = -1;
    queue->count = 0;
}

void empty(Queue *queue) {

    free(queue->ids);
    free(queue);
}

int bBFS(ind *index_in,
         ind *index_out,
         list_node *buffer_in,
         list_node *buffer_out,
         uint32_t start,
         uint32_t end,
         Queue *frontierF,
         Queue *frontierB,
         uint32_t version,
         int thread_id,
         int curr_scc) {

    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT, childrenF = 0, childrenB = 0;
    int i = 0, steps = 0, curr_steps = 0, min_steps = -1, path = 0;
    ptrdiff_t offset = 0;
    int counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, stepsF = 0, stepsB = 0;

    if (start == end)   // an o komvos ekkinhshs einai o komvos stoxos tote steps=0
        return 0;

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
                        if (successor != DEFAULT) {

                            if(curr_scc < 0){

                                if(neighbors->edgeProperty[i] <= version) {

                                    if (index_out[successor].visited[thread_id] !=
                                        version) {  // an den ton exei epispeftei o idios
                                        index_out[successor].visited[thread_id] = version; // ton episkeptetai

                                        if (index_in[successor].visited[thread_id] ==
                                            version) {   // goal afou ton exei episkeptei o allos
                                            restartQueue(frontierF);
                                            restartQueue(frontierB);
                                            return stepsB + stepsF;

                                        } else {   // alliws eisagetai sto synoro
                                            enq(frontierF, successor);
                                            counterFS++;
                                            childrenF += index_out[successor].num_of_children;
                                        }
                                    }
                                }
                            }
                            else {
                                if (curr_scc != DEFAULT) {  // periptwsh anazhthshs sto idio scc

                                    if (scc->id_belongs_to_component[successor] == curr_scc) {

                                        if (index_out[successor].visited[thread_id] !=
                                            version) {  // an den ton exei epispeftei o idios
                                            index_out[successor].visited[thread_id] = version;         // ton episkeptetai

                                            if (index_in[successor].visited[thread_id] ==
                                                version) {   // goal afou ton exei episkeptei o allos
                                                restartQueue(frontierF);
                                                restartQueue(frontierB);
                                                return stepsB + stepsF;

                                            } else {   // alliws eisagetai sto synoro
                                                enq(frontierF, successor);
                                                counterFS++;
                                                childrenF += index_out[successor].num_of_children;
                                            }

                                        }

                                    }

                                } else {    // periptwsh anazhthshs se olo ton grafo

                                    if (isReachableGrailIndex(grail, successor, end, scc) != NO) {

                                        if (index_out[successor].visited[thread_id] !=
                                            version) {  // an den ton exei epispeftei o idios
                                            index_out[successor].visited[thread_id] = version; // ton episkeptetai

                                            if (index_in[successor].visited[thread_id] ==
                                                version) {   // goal afou ton exei episkeptei o allos
                                                restartQueue(frontierF);
                                                restartQueue(frontierB);
                                                return stepsB + stepsF;

                                            } else {   // alliws eisagetai sto synoro
                                                enq(frontierF, successor);
                                                counterFS++;
                                                childrenF += index_out[successor].num_of_children;
                                            }

                                        }
                                    }
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
                        if (successor != DEFAULT) {
                            if(curr_scc < 0){

                                if(neighbors->edgeProperty[i] <= version) {

                                    if (index_in[successor].visited[thread_id] != version) {   // an den ton exei episkeptei o idios
                                        index_in[successor].visited[thread_id] = version;  // ton episkeptetai

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
                                }
                            }
                            else {
                                if (curr_scc != DEFAULT) {  // periptwsh anazhthshs sto idio scc

                                    if (scc->id_belongs_to_component[successor] == curr_scc) {

                                        if (index_in[successor].visited[thread_id] !=
                                            version) {   // an den ton exei episkeptei o idios
                                            index_in[successor].visited[thread_id] = version;  // ton episkeptetai

                                            if (index_out[successor].visited[thread_id] ==
                                                version) {  // goal afou ton exei episkeptei o allos

                                                restartQueue(frontierB);
                                                restartQueue(frontierF);
                                                return stepsF + stepsB;

                                            } else {    // alliws eisagetai sto synoro
                                                enq(frontierB, successor);
                                                counterBS++;
                                                childrenB += index_in[successor].num_of_children;
                                            }

                                        }

                                    }

                                } else {    // periptwsh anazhthshs se olo ton grafo

                                    if (isReachableGrailIndex(grail, start, successor, scc) != NO) {

                                        if (index_in[successor].visited[thread_id] !=
                                            version) {   // an den ton exei episkeptei o idios
                                            index_in[successor].visited[thread_id] = version;  // ton episkeptetai

                                            if (index_out[successor].visited[thread_id] ==
                                                version) {  // goal afou ton exei episkeptei o allos

                                                restartQueue(frontierB);
                                                restartQueue(frontierF);
                                                return stepsF + stepsB;

                                            } else {    // alliws eisagetai sto synoro
                                                enq(frontierB, successor);
                                                counterBS++;
                                                childrenB += index_in[successor].num_of_children;
                                            }

                                        }
                                    }

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

