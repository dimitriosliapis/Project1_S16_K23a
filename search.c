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

/* ----------DEN XRHSΙMOPOIOYNTAI---------- */

int search(Queue *queue, uint32_t id) {

    q_Node *curr = NULL;
    curr = queue->first;
    while (curr != NULL) {
        if (curr->id == id)
            return FOUND;
        curr = curr->next;
    }
    return NOT_FOUND;
}

uint32_t hash(uint32_t x) {

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

void resetHash(hNode *hashTable) {

    int i = 0;
    for (i = 0; i < H_DEFAULT; i++) {

        hashTable[i].id = DEFAULT;
        hashTable[i].steps = 0;
        hashTable[i].next = NULL;
    }
}

int isIn(hNode *hashTable, uint32_t id) {

    int pos = 0;
    hNode *tmp = NULL;

    pos = hash(id) % H_DEFAULT;
    tmp = hashTable[pos].next;

    while (tmp->next != NULL) {
        if (hashTable[pos].id == id)
            return FOUND;
        tmp = tmp->next;
    }
    return NOT_FOUND;
}

void addToHash(hNode *hashTable, uint32_t id, uint32_t steps) {

    int pos = 0;
    hNode *new = NULL, *tmp = NULL;

    pos = hash(id) % H_DEFAULT;

    if (hashTable[pos].id == DEFAULT) {
        hashTable[pos].id = id;
        hashTable[pos].steps = steps;
        return;
    }

    new = malloc(sizeof(hNode));

    new->id = id;
    new->steps = steps;
    new->next = NULL;

    tmp = hashTable[pos].next;

    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    tmp->next = new;
}

/* ---------------------------------------- */

int bBFS(ind *index_in, ind *index_out, list_node *buffer_in, list_node *buffer_out, uint32_t start, uint32_t end) {

    Queue *frontierF = NULL, *frontierB = NULL;
    list_node *neighbors = NULL;
    uint32_t node = DEFAULT, successor = DEFAULT;
    int i = 0, j = 0, counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, steps = 0;
    ptrdiff_t offset = 0;

    if (start == end)
        return 0;

    frontierF = createQueue();
    frontierB = createQueue();

    index_out[start].visited = 1;
    index_out[start].steps = steps;
    push(frontierF, start);
    counterF++;

    index_in[end].visited = 1;
    index_in[end].steps = steps;
    push(frontierB, end);
    counterB++;

    while (!isEmpty(frontierF) || !isEmpty(frontierB)) {

        while (counterF != 0) {

            node = pop(frontierF);
            steps = index_out[node].steps;

            offset = getListHead(index_out, node);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighbors = buffer_out + offset;
                while (i < N) {

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (index_out[successor].visited == 0) {
                            index_out[successor].visited = 1;
                            index_out[successor].steps = steps + 1;

                            if (index_in[successor].visited == 1) {   // goal
                                empty(frontierF);
                                empty(frontierB);
                                return index_in[successor].steps + index_out[successor].steps;
                            } else {
                                push(frontierF, successor);
                                counterFS++;
                            }
                        }
                    } else
                        break;

                    i++;
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

        while (counterB != 0) {

            node = pop(frontierB);
            steps = index_in[node].steps;

            offset = getListHead(index_in, node);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighbors = buffer_in + offset;
                while (i < N) {

                    successor = neighbors->neighbor[i];
                    if (successor != DEFAULT) {

                        if (index_in[successor].visited == 0) {
                            index_in[successor].visited = 1;
                            index_in[successor].steps = steps + 1;

                            if (index_out[successor].visited == 1) {   // goal
                                empty(frontierB);
                                empty(frontierF);
                                return index_out[successor].steps + index_in[successor].steps;
                            } else {
                                push(frontierB, successor);
                                counterBS++;
                            }
                        }
                    } else
                        break;

                    i++;
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

    free(frontierF);
    free(frontierB);
    return -1;
}
