#include "search.h"

Queue *createQueue() {

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
        queue->ids = realloc(queue->ids, queue->size * 2 * sizeof(uint32_t));
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


int bBFS(ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t start, uint32_t end, Queue *frontierF, Queue *frontierB, ht_Node *exploredF, ht_Node *exploredB) {

    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT;
    int i = 0, counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, stepsF = 0, stepsB = 0;
    ptrdiff_t offset = 0;

    if (start == end)   // an o komvos ekkinhshs einai o komvos stoxos tote steps=0
        return 0;

    insert(exploredF, start, HT_BIG);
    enq(frontierF, start);
    counterF++;

    insert(exploredB, end, HT_BIG);
    enq(frontierB, end);
    counterB++;

    while (!isEmpty(frontierF) && !isEmpty(frontierB)) {    // oso ta 2 synora den einai adeia

        stepsF++;
        while (counterF != 0) { // epanalhpsh gia tous komvous poy mphkan sto synoro sto prohgoumeno vathos

            node = deq(frontierF);  // deq

            offset = getListHead(index_out, node);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighbors = buffer_out + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (search(exploredF, successor, HT_BIG) == NOT_FOUND) {
                            insert(exploredF, successor, HT_BIG);

                            if (search(exploredB, successor, HT_BIG) == FOUND) {
                                restartQueue(frontierF);
                                restartQueue(frontierB);
                                reinitialize(exploredF, HT_BIG);
                                reinitialize(exploredB, HT_BIG);
                                return stepsB + stepsF;
                            } else {    // alliws eisagwgh sto synoro
                                enq(frontierF, successor);
                                counterFS++;
                            }
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

                i = 0;
            }

            counterF--;
        }

        counterF = counterFS;
        counterFS = 0;

        stepsB++;
        while (counterB != 0) { // epanalhpsh gia tous komvous poy mphkan sto synoro sto prohgoumeno vathos

            node = deq(frontierB);  // deq

            offset = getListHead(index_in, node);
            if (offset != -1) { // mporei na mhn exei geitones

                neighbors = buffer_in + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (search(exploredB, successor, HT_BIG) == NOT_FOUND) {
                            insert(exploredB, successor, HT_BIG);

                            if (search(exploredF, successor, HT_BIG) == FOUND) {
                                restartQueue(frontierB);
                                restartQueue(frontierF);
                                reinitialize(exploredB, HT_BIG);
                                reinitialize(exploredF, HT_BIG);
                                return stepsF + stepsB;
                            } else {    // alliws eisagwgh sto synoro
                                enq(frontierB, successor);
                                counterBS++;
                            }
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

                i = 0;
            }

            counterB--;
        }

        counterB = counterBS;
        counterBS = 0;
    }

    restartQueue(frontierF);
    restartQueue(frontierB);
    reinitialize(exploredF, HT_BIG);
    reinitialize(exploredB, HT_BIG);
    return -1;  // an den vrethei monopati epistrefei -1
}
