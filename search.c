#include "search.h"

Queue *createQueue() {

    Queue *queue = NULL;
    queue = malloc(sizeof(Queue));
    if (queue == NULL)
        return NULL;
    else {
        queue->first = NULL;
        queue->last = NULL;
        return queue;
    }
}

int isEmpty(Queue *queue) {

    return (queue->first == NULL);
}

int enq(Queue *queue, uint32_t id) {

    q_Node *new = NULL;
    if (queue == NULL)
        return -1;
    new = malloc(sizeof(q_Node));
    new->id = id;
    new->next = NULL;

    if (queue->first == NULL)
        queue->first = new;
    else
        queue->last->next = new;
    queue->last = new;
    return 0;
}

uint32_t deq(Queue *queue) {

    q_Node *out = NULL;
    uint32_t id = 0;

    if (isEmpty(queue)) return DEFAULT;

    out = queue->first;
    queue->first = out->next;
    id = out->id;

    if (queue->first == NULL)
        queue->last = NULL;

    free(out);
    return id;
}

void restartQueue(Queue *queue) {

    q_Node *curr = queue->first, *prev = NULL;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    queue->first = NULL;
    queue->last = NULL;
}

void empty(Queue *queue) {

    q_Node *curr = queue->first, *prev = NULL;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    free(queue);
}


int bBFS(ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t start, uint32_t end, Queue *frontierF, Queue *frontierB, ht_Node *exploredF, ht_Node *exploredB) {

    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT;
    int i = 0, j = 0, counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, stepsF = 0, stepsB = 0;
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
