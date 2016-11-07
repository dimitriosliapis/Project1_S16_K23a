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

int push(Queue *queue, uint32_t id, int steps) {

    q_Node *new = NULL;
    if (queue == NULL)
        return -1;
    new = malloc(sizeof(q_Node));
    new->id = id;
    new->next = NULL;
    new->steps = steps;

    if (queue->first == NULL)
        queue->first = new;
    else
        queue->last->next = new;
    queue->last = new;
    return 0;
}

uint32_t pop(Queue *queue, int *steps) {

    q_Node *out = NULL;
    uint32_t id = 0;

    if (isEmpty(queue)) return DEFAULT;

    out = queue->first;
    queue->first = out->next;
    id = out->id;
    *steps = out->steps;

    free(out);
    return id;
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
    list_node *neighborsF = NULL, *neighborsB = NULL;
    uint32_t nodeF = DEFAULT, nodeB = DEFAULT, successor = DEFAULT;
    int i = 0, steps = 0;
    ptrdiff_t offset = 0;

    if (start == end)
        return 0;

    frontierF = createQueue();

    frontierB = createQueue();

    index_out[start].visited = 1;
    push(frontierF, start, steps);
    index_out[start].steps = steps;

    index_in[end].visited = 1;
    push(frontierB, end, steps);
    index_in[end].steps = steps;

    while (!isEmpty(frontierF) || !isEmpty(frontierB)) {

        if (!isEmpty(frontierF)) {

            nodeF = pop(frontierF, &steps);
//            if (index_in[nodeF].visited == 1)    // goal
//                return index_in[nodeF].steps + steps;
            steps++;

            offset = getListHead(index_out, nodeF);
            if (offset != -1) {     // mporei na mhn exei geitones

                neighborsF = buffer_out + offset;
                while (i < N) {

                    successor = neighborsF->neighbor[i];
                    if (successor != DEFAULT) {

                        if (index_out[successor].visited == 0) {
                            index_out[successor].visited = 1;

                            if (index_in[successor].visited == 1)    // goal
                                return index_in[successor].steps + steps;
                            else {
                                push(frontierF, successor, steps);
                                index_out[successor].steps = steps;
                            }
                        }
                    } else
                        break;

                    i++;
                    if (i == N && neighborsF->nextListNode != -1) {
                        neighborsF = buffer_out + neighborsF->nextListNode;
                        i = 0;
                    }
                }

                i = 0;
            }
        }

        if (!isEmpty(frontierB)) {

            nodeB = pop(frontierB, &steps);
//            if (index_out[nodeB].visited == 1)  // goal
//                return index_out[nodeB].steps + steps;
            steps++;

            if (index_out[nodeB].visited == 1) {
                return index_in[nodeB].steps + index_out[nodeB].steps;
            }

            offset = getListHead(index_in, nodeB);
            if (offset != -1) {

                neighborsB = buffer_in + offset;
                while (i < N) {

                    successor = neighborsB->neighbor[i];
                    if (successor != DEFAULT) {

                        if (index_in[successor].visited == 0) {
                            index_in[successor].visited = 1;

                            if (index_out[successor].visited == 1)  // goal
                                return index_out[successor].steps + steps;
                            else {
                                push(frontierB, successor, steps);
                                index_in[successor].steps = steps;
                            }
                        }
                    } else
                        break;

                    i++;
                    if (i == N && neighborsB->nextListNode != -1) {
                        neighborsB = buffer_in + neighborsB->nextListNode;
                        i = 0;
                    }
                }

                i = 0;
            }
        }
    }

    return -1;
}
