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

int push(Queue *queue, uint32_t id) {

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

uint32_t pop(Queue *queue) {

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

void empty(Queue *queue) {

    q_Node *curr = queue->first, *prev = NULL;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    free(queue);
}


int bBFS(ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t start, uint32_t end) {

    Queue *frontierF = NULL, *frontierB = NULL;
    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT;
    int i = 0, j = 0, counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, steps = 0;
    ptrdiff_t offset = 0;

    if (start == end)   // an o komvos ekkinhshs einai o komvos stoxos tote steps=0
        return 0;

    frontierF = createQueue();  // synoro tou bfs apo thn arxh pros ton stoxo
    frontierB = createQueue();  // synoro tou bfs apo ton stoxo pros thn arxh

    index_out[start].visited = 1;
    index_out[start].steps = steps;
    push(frontierF, start);
    counterF++;

    index_in[end].visited = 1;
    index_in[end].steps = steps;
    push(frontierB, end);
    counterB++;

    while (!isEmpty(frontierF) && !isEmpty(frontierB)) {    // oso ta 2 synora den einai adeia

        while (counterF != 0) { // epanalhpsh gia tous komvous poy mphkan sto synoro sto prohgoumeno vathos

            node = pop(frontierF);  // pop
            steps = index_out[node].steps;

            offset = getListHead(index_out, node);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighbors = buffer_out + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (index_out[successor].visited == 0) {    // an den ton exei episkeftei
                            index_out[successor].visited = 1;   // ton markarisma
                            index_out[successor].steps = steps + 1;

                            if (index_in[successor].visited == 1) {   // an ton exei episkeftei o allos bfs GOAL!
                                empty(frontierF);
                                empty(frontierB);
                                return index_in[successor].steps + index_out[successor].steps;
                            } else {    // alliws eisagwgh sto synoro
                                push(frontierF, successor);
                                counterFS++;
                            }
                        }
                    } else
                        break;

                    i++;
                    if (i == N && neighbors->nextListNode != -1) {  // an exei ki allous geitones se epomeno listnode synexizei
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

        while (counterB != 0) { // epanalhpsh gia tous komvous poy mphkan sto synoro sto prohgoumeno vathos

            node = pop(frontierB);
            steps = index_in[node].steps;

            offset = getListHead(index_in, node);
            if (offset != -1) { // mporei na mhn exei geitones

                neighbors = buffer_in + offset;
                while (i < N) { // gia kathe geitona

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (index_in[successor].visited == 0) { // an den ton exei episkeftei
                            index_in[successor].visited = 1;    // ton markarisma
                            index_in[successor].steps = steps + 1;

                            if (index_out[successor].visited == 1) {    // an ton exei episkeftei o allos bfs GOAL!
                                empty(frontierB);
                                empty(frontierF);
                                return index_out[successor].steps + index_in[successor].steps;
                            } else {    // alliws eisagwgh sto synoro
                                push(frontierB, successor);
                                counterBS++;
                            }
                        }
                    } else
                        break;

                    i++;
                    if (i == N && neighbors->nextListNode != -1) {  // an exei ki allous geitones se epomeno listnode synexizei
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

    empty(frontierF);
    empty(frontierB);
    return -1;  // an den vrethei monopati epistrefei -1
}
