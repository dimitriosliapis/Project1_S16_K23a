#include "search.h"

Queue *createQueue() {

    int i = 0;
    Queue *queue = NULL;
    queue = malloc(sizeof(Queue));

    if (queue == NULL)
        return NULL;
    else {
        queue->size = QUEUE_SIZE;
        queue->entries = malloc(QUEUE_SIZE * sizeof(qEntry));
        for (i = 0; i < queue->size; i++) {
            queue->entries[i].id = DEFAULT;
            queue->entries[i].steps = -1;
        }
        queue->first = 0;
        queue->last = -1;
        queue->count = 0;
        return queue;
    }
}

int isEmpty(Queue *queue) {

    return (queue->count == 0);
}

int enq(Queue *queue, uint32_t id, int steps) {

    qEntry *new = NULL;

    if (queue->count >= queue->size) {
        new = realloc(queue->entries, queue->size * 2 * sizeof(qEntry));
        queue->entries = new;
        queue->size *= 2;
    }

    queue->last = (queue->last + 1) % queue->size;
    queue->entries[queue->last].id = id;
    queue->entries[queue->last].steps = steps;
    queue->count++;

    return 0;
}

uint32_t deq(Queue *queue, int *steps) {

    uint32_t id = DEFAULT;


    if (queue->count == 0)
        return DEFAULT;

    id = queue->entries[queue->first].id;
    *steps = queue->entries[queue->first].steps;
    queue->first = (queue->first + 1) % queue->size;
    queue->count--;
    return id;
}

uint32_t qpeek(Queue *queue) {

    if (queue->count == 0)
        return DEFAULT;

    return queue->entries[queue->first].id;
}

int nsteps(Queue *queue, uint32_t id) {

    int i = 0;

    for (i = 0; i < queue->size; i++) {
        if (queue->entries[i].id == id)
            return queue->entries[i].steps;
    }
    return -1;
}

void restartQueue(Queue *queue) {

    int i = 0;

    for (i = 0; i < queue->size; i++) {
        queue->entries[i].id = DEFAULT;
        queue->entries[i].steps = -1;
    }

    queue->first = 0;
    queue->last = -1;
    queue->count = 0;
}

void empty(Queue *queue) {

    free(queue->entries);
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
         ht_Node *exploredF,
         ht_Node *exploredB,
         uint32_t version) {

    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT, childrenF = 0, childrenB = 0;
    int i = 0, steps = 0, curr_steps = 0, min_steps = -1, path = 0;
    ptrdiff_t offset = 0;
//    int fprint = 0;

    if (start == end)   // an o komvos ekkinhshs einai o komvos stoxos tote steps=0
        return 0;

//    if (start == 1180829 && end == 1268954)
//        fprint = 1;

    bfs_insert(exploredF, start, HT_BIG, steps, version);
    enq(frontierF, start, steps);
    childrenF = index_out[start].num_of_children;

    bfs_insert(exploredB, end, HT_BIG, steps, version);
    enq(frontierB, end, steps);
    childrenB = index_in[end].num_of_children;

    while (!isEmpty(frontierF) && !isEmpty(frontierB)) {    // oso ta 2 synora den einai adeia

        if (!isEmpty(frontierF) && childrenF <= childrenB) {

            node = deq(frontierF, &steps);  // dequeue
            steps++;

//            if (fprint == 1)
//                printf("Forward BFS expanded %d\n", node);

            offset = getListHead(index_out, node);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighbors = buffer_out + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        // an den ton exei epispeftei o idios
                        if (search(exploredF, successor, HT_BIG, version) == NOT_FOUND) {
                            bfs_insert(exploredF, successor, HT_BIG, steps, version);   // ton episkeptetai

                            // goal afou ton exei episkeptei o allos
                            if (search(exploredB, successor, HT_BIG, version) == FOUND) {

                                curr_steps = num_steps(exploredB, successor, HT_BIG, version);
                                if (min_steps == -1) {
                                    min_steps = curr_steps;
                                }else if (curr_steps < min_steps) {
                                    min_steps = curr_steps;
                                }

                            } else    // alliws eisagetai sto synoro
                                enq(frontierF, successor, steps);

                        }

                    } else
                        break;

                    i++;

                    if (i == N &&
                        neighbors->nextListNode != -1) {  // an exei ki allous geitones se epomeno listnode synexizei
                        neighbors = buffer_out + neighbors->nextListNode;
                        i = 0;
                    }
                }

                if (min_steps != -1) {
                    path = min_steps + steps;
                    restartQueue(frontierF);
                    restartQueue(frontierB);
                    return path;
                }

                i = 0;
            }
        }

        if (!isEmpty(frontierF))
            childrenF = index_out[qpeek(frontierF)].num_of_children;
        else
            childrenF = DEFAULT;
        min_steps = -1;

        if (!isEmpty(frontierB) && childrenB < childrenF) {

            node = deq(frontierB, &steps);  // dequeue
            steps++;

//            if (fprint == 1)
//                printf("Backward BFS expanded %d\n", node);

            offset = getListHead(index_in, node);
            if (offset != -1) { // mporei na mhn exei geitones

                neighbors = buffer_in + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        // an den ton exei episkeptei o idios
                        if (search(exploredB, successor, HT_BIG, version) == NOT_FOUND) {
                            bfs_insert(exploredB, successor, HT_BIG, steps, version);   // ton episkeptetai

                            // goal afou ton exei episkeptei o allos
                            if (search(exploredF, successor, HT_BIG, version) == FOUND) {

                                curr_steps = num_steps(exploredF, successor, HT_BIG, version);
                                if (min_steps == -1) {
                                    min_steps = curr_steps;
                                } else if (curr_steps < min_steps) {
                                    min_steps = curr_steps;
                                }

                            } else    // alliws eisagetai sto synoro
                                enq(frontierB, successor, steps);

                        }

                    } else
                        break;

                    i++;

                    if (i == N &&
                        neighbors->nextListNode != -1) {  // an exei ki allous geitones se epomeno listnode synexizei
                        neighbors = buffer_in + neighbors->nextListNode;
                        i = 0;
                    }
                }

                if (min_steps != -1) {
                    path = min_steps + steps;
                    restartQueue(frontierB);
                    restartQueue(frontierF);
                    return path;
                }

                i = 0;
            }
        }

        if (!isEmpty(frontierB))
            childrenB = index_in[qpeek(frontierB)].num_of_children;
        else
            childrenB = DEFAULT;
        min_steps = -1;
    }

    restartQueue(frontierF);
    restartQueue(frontierB);

    return -1;  // an den vrethei monopati epistrefei -1
}
