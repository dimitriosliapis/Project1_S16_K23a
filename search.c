#include "search.h"
#include "scc.h"
#include "grail.h"

extern SCC *scc;
extern GrailIndex *grail;

// Create circular FIFO queue
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

// Check if queue is empty
int isEmpty(Queue *queue) {

    return (queue->count == 0);
}

// Push element at the end of queue
int enq(Queue *queue, uint32_t id) {

    if (queue->count >= queue->size) {

        queue->ids = realloc(queue->ids, queue->size * 2 * sizeof(uint32_t));

        memmove(queue->ids + (queue->size + queue->first),
                queue->ids + queue->first, (queue->size - queue->first) * sizeof(uint32_t));

        queue->first = queue->first + queue->size;
        queue->size *= 2;
    }

    queue->last = (queue->last + 1) % queue->size;
    queue->ids[queue->last] = id;
    queue->count++;

    return 0;
}

// Pop first element in queue
uint32_t deq(Queue *queue) {

    uint32_t id = DEFAULT;

    if (queue->count == 0)
        return DEFAULT;

    id = queue->ids[queue->first];
    queue->first = (queue->first + 1) % queue->size;
    queue->count--;
    return id;
}

// Reinitialize queue
void restartQueue(Queue *queue) {

    queue->first = 0;
    queue->last = -1;
    queue->count = 0;
}

// Free queue
void empty(Queue *queue) {

    free(queue->ids);
    free(queue);
}

// Bidirectional Breadth First Search algorithm to find path between two given nodes and return steps
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
    int i = 0, counterF = 0, counterFS = 0, counterB = 0, counterBS = 0, stepsF = 0, stepsB = 0, reachable;
    ptrdiff_t offset = 0;

    if (start == end)   // if start node is same as goal node then return 0
        return 0;

    index_out[start].visited[thread_id] = version;
    enq(frontierF, start);
    childrenF = index_out[start].num_of_children;
    counterF++;

    index_in[end].visited[thread_id] = version;
    enq(frontierB, end);
    childrenB = index_in[end].num_of_children;
    counterB++;

    while (!isEmpty(frontierF) && !isEmpty(frontierB)) {    // while frontiers are not empty

        if (childrenF <= childrenB) {   // choose forward bfs if less nodes are going to be expanded

            stepsF++;
            while (counterF != 0) {     // iteration for all nodes in the same depth

                node = deq(frontierF);  // pop first node in frontier

                offset = getListHead(index_out, node);
                if (offset != -1) {     // check if node has neighbors

                    neighbors = buffer_out + offset;
                    while (i < N) {     // for each neighbor

                        successor = neighbors->neighbor[i];
                        if (successor != DEFAULT) {

                            if (curr_scc < 0) {   // dynamic graph case

                                // if neighbor exists in current version
                                if (neighbors->edgeProperty[i] <= version) {

                                    // if neighbor is not visited by the current bfs then visit
                                    if (index_out[successor].visited[thread_id] != version) {
                                        index_out[successor].visited[thread_id] = version;

                                        // if neighbor is visited by the other bfs then goal
                                        if (index_in[successor].visited[thread_id] == version) {
                                            restartQueue(frontierF);
                                            restartQueue(frontierB);
                                            return stepsB + stepsF;

                                        } else {   // else push in frontier
                                            enq(frontierF, successor);
                                            counterFS++;
                                            childrenF += index_out[successor].num_of_children;
                                        }
                                    }
                                }
                            } else {    // static graph case

                                if (curr_scc != DEFAULT) {  // search in same scc case

                                    // if neighbor exists in current scc
                                    if (scc->id_belongs_to_component[successor] == curr_scc) {

                                        // if neighbor is not visited by the current bfs then visit
                                        if (index_out[successor].visited[thread_id] != version) {
                                            index_out[successor].visited[thread_id] = version;

                                            // if neighbor is visited by the other bfs then goal
                                            if (index_in[successor].visited[thread_id] == version) {
                                                restartQueue(frontierF);
                                                restartQueue(frontierB);
                                                return stepsB + stepsF;

                                            } else {   // else push in frontier
                                                enq(frontierF, successor);
                                                counterFS++;
                                                childrenF += index_out[successor].num_of_children;
                                            }

                                        }

                                    }

                                } else {    // search in whole graph case

                                    reachable = isReachableGrailIndex(grail, successor, end, scc);

                                    // if goal may be reachable from neighbor
                                    if (reachable != NO) {

                                        if (reachable == YES)
                                            curr_scc = scc->id_belongs_to_component[successor];

                                        // if neighbor is not visited by the current bfs then visit
                                        if (index_out[successor].visited[thread_id] != version) {
                                            index_out[successor].visited[thread_id] = version;

                                            // if neighbor is visited by the other bfs then goal
                                            if (index_in[successor].visited[thread_id] == version) {
                                                restartQueue(frontierF);
                                                restartQueue(frontierB);
                                                return stepsB + stepsF;

                                            } else {   // else push in frontier
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

                        // if there are more neighbors in next listnode continue examining
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

        if (childrenB < childrenF) {    // choose backward bfs if less nodes are going to be expanded

            stepsB++;
            while (counterB != 0) {     // iteration for all nodes in the same depth

                node = deq(frontierB);  // pop first node in frontier

                offset = getListHead(index_in, node);
                if (offset != -1) {     // check if node has neighbors

                    neighbors = buffer_in + offset;
                    while (i < N) {     // for each neighbor

                        successor = neighbors->neighbor[i];
                        if (successor != DEFAULT) {

                            if (curr_scc < 0) {   // dynamic graph case

                                // if neighbor exists in current version
                                if (neighbors->edgeProperty[i] <= version) {

                                    // if neighbor is not visited by the current bfs then visit
                                    if (index_in[successor].visited[thread_id] != version) {
                                        index_in[successor].visited[thread_id] = version;

                                        // if neighbor is visited by the other bfs then goal
                                        if (index_out[successor].visited[thread_id] == version) {
                                            restartQueue(frontierB);
                                            restartQueue(frontierF);
                                            return stepsF + stepsB;

                                        } else {    // else push in frontier
                                            enq(frontierB, successor);
                                            counterBS++;
                                            childrenB += index_in[successor].num_of_children;
                                        }

                                    }
                                }
                            } else {    // static graph case

                                if (curr_scc != DEFAULT) {  // search in same scc case

                                    // if neighbor exists in current scc
                                    if (scc->id_belongs_to_component[successor] == curr_scc) {

                                        // if neighbor is not visited by the current bfs then visit
                                        if (index_in[successor].visited[thread_id] != version) {
                                            index_in[successor].visited[thread_id] = version;

                                            // if neighbor is visited by the other bfs then goal
                                            if (index_out[successor].visited[thread_id] == version) {
                                                restartQueue(frontierB);
                                                restartQueue(frontierF);
                                                return stepsF + stepsB;

                                            } else {    // else push in frontier
                                                enq(frontierB, successor);
                                                counterBS++;
                                                childrenB += index_in[successor].num_of_children;
                                            }

                                        }

                                    }

                                } else {    // search in whole graph case

                                    reachable = isReachableGrailIndex(grail, start, successor, scc);

                                    // if goal may be reachable from neighbor
                                    if (reachable != NO) {

                                        if (reachable == YES)
                                            curr_scc = scc->id_belongs_to_component[successor];

                                        // if neighbor is not visited by the current bfs then visit
                                        if (index_in[successor].visited[thread_id] != version) {
                                            index_in[successor].visited[thread_id] = version;

                                            // if neighbor is visited by the other bfs then goal
                                            if (index_out[successor].visited[thread_id] == version) {
                                                restartQueue(frontierB);
                                                restartQueue(frontierF);
                                                return stepsF + stepsB;

                                            } else {    // else push in frontier
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

                        // if there are more neighbors in next listnode continue examining
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

    return -1;  // if path is not found then return -1
}
