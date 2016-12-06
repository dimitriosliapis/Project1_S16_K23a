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


int bBFS(ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t start, uint32_t end, Queue *frontierF, Queue *frontierB, ht_Node *exploredF, ht_Node *exploredB, uint32_t version) {

    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT, optimal = DEFAULT;
    int i = 0, steps = 0, curr_steps = 0, min_steps = -1, path = 0;
    ptrdiff_t offset = 0;

    if (start == end)   // an o komvos ekkinhshs einai o komvos stoxos tote steps=0
        return 0;

    insert(exploredF, start, HT_BIG, version);
    enq(frontierF, start, steps);

    insert(exploredB, end, HT_BIG, version);
    enq(frontierB, end, steps);

    while (!isEmpty(frontierF) && !isEmpty(frontierB)) {    // oso ta 2 synora den einai adeia

        if (!isEmpty(frontierF)) {

            node = deq(frontierF, &steps);  // dequeue
            steps++;

            offset = getListHead(index_out, node);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighbors = buffer_out + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (search(exploredF, successor, HT_BIG, version) == NOT_FOUND) {
                            insert(exploredF, successor, HT_BIG, version);

                            if (search(exploredB, successor, HT_BIG, version) == FOUND) {

                                curr_steps = nsteps(frontierB, successor);
                                if (min_steps == -1) {
                                    min_steps = curr_steps;
                                    optimal = successor;
                                } else if (curr_steps < min_steps) {
                                    min_steps = curr_steps;
                                    optimal = successor;
                                }

                            } else      // alliws eisagwgh sto synoro
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

        min_steps = -1;

        if (!isEmpty(frontierB)) {

            node = deq(frontierB, &steps);  // dequeue
            steps++;

            offset = getListHead(index_in, node);
            if (offset != -1) { // mporei na mhn exei geitones

                neighbors = buffer_in + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (search(exploredB, successor, HT_BIG, version) == NOT_FOUND) {
                            insert(exploredB, successor, HT_BIG, version);

                            if (search(exploredF, successor, HT_BIG, version) == FOUND) {

                                curr_steps = nsteps(frontierF, successor);
                                if (min_steps == -1) {
                                    min_steps = curr_steps;
                                    optimal = successor;
                                } else if (curr_steps < min_steps) {
                                    min_steps = curr_steps;
                                    optimal = successor;
                                }

                            } else      // alliws eisagwgh sto synoro
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

        min_steps = -1;
    }

    restartQueue(frontierF);
    restartQueue(frontierB);
    return -1;  // an den vrethei monopati epistrefei -1
}