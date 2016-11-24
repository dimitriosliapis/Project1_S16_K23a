#include "cc.h"


void push(Stack *stack, uint32_t id){

    sNode *new = malloc(sizeof(sNode));

    new->id = id;
    new->next = stack->first;

    stack->first = new;
}

uint32_t pop(Stack *stack){

    sNode *rem = stack->first;
    uint32_t id;

    if(rem == NULL) return DEFAULT;

    id = rem->id;
    stack->first = rem->next;
    free(rem);
    return id;
}

void deleteStack(Stack *stack, sNode *current){

    if(stack == NULL) return;
    if(stack->first == NULL){
        free(stack);
        return;
    }
    if(current->next == NULL){
        free(current);
        return;
    }
    deleteStack(stack, current->next);
}