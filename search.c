#include "search.h"

char* idToStr(uint32_t id){

    uint32_t rem = 0;
    char *ret = NULL, c;
    size_t size = 1; // +1 for NULL terminator

    if(id == 0) return NULL;

    rem = id;
    while(rem != 0){
        rem /= 10;
        size++;
    }

    ret = malloc(size * sizeof(char));
    memset(ret,'\0',size);

    sprintf(ret,"%u",id);

    return ret;
}



char* append(char *a, char *b){

    int lenA = strlen(a);
    int lenB = strlen(b);
    char *ret = malloc((lenA+lenB+3)*sizeof(char));

    memset(ret,'\0', lenA+lenB+1);
    strncat(ret,a,lenA);
    strncat(ret,"->",2);
    strncat(ret,b,lenB);

    return ret;
}

uint32_t *nextBarrier(list_node *current){
    return current->neighbor;
}

int isIn(uint32_t id, uint32_t *ar){

    int i = 0;

    for(i = 0; i < N; i++){
        if(ar[i] == id) return FOUND;
    }

    return NOT_FOUND;
}

uint32_t hash(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

void resetHash(){

    int i = 0;

    for(i = 0; i < H_DEFAULT; i++){

        hashTable[i].id = DEFAULT;
        hashTable[i].next = NULL;
        hashTable[i].distance = -1;
        hashTable[i].parent_id = DEFAULT;
    }
}

void addToHash(uint32_t id){

    int pos = 0;
    hNode *new = NULL, *tmp = NULL;

    pos = hash(id)%H_DEFAULT;

    if(hashTable[pos].id == DEFAULT){
        hashTable[pos].id = id;
        return;
    }
    //ELEGXOS AN UPARXEI hashtable[pos].id == id

    new = malloc(sizeof(hNode));

    new->id = id;
    new->parent_id = DEFAULT;
    new->distance = -1;
    new->next = NULL;

    tmp = hashTable[pos].next;

    while(tmp->next != NULL){
        tmp = tmp->next;
    }

    tmp->next = new;

    /////////////////////
}

int createQueue(){

    int i = 0;

    if (queue == NULL){
        queue = malloc(sizeof(Queue));
        queue->first = NULL;
        queue->last = NULL;
    }
    return OK_SUCCESS;
}

int isEmptyQueue(){

    if(queue == NULL) return 1;
    return 0;
}

void enQueue(uint32_t id){

    q_Node *new = NULL;

    if(isEmptyQueue()){
        createQueue();
    }
    new = malloc(sizeof(q_Node));
    new->id = id;
    new->next = NULL;

    if(queue->first == NULL){
        queue->first = new;
    }
    queue->last = new;
}

uint32_t deQueue(){

    q_Node *out = NULL;
    uint32_t id = 0;

    if(isEmptyQueue()) return DEFAULT;

    out = queue->first;

    queue->first = out->next;
    id = out->id;

    free(out);
    return id;

}

int BFS(ptrdiff_t *index, list_node *buffer, uint32_t start, uint32_t end){


}

char* bBFS(ptrdiff_t *indexIn, ptrdiff_t *indexOut, list_node *bufferOut, list_node *bufferIn, uint32_t start, uint32_t end, uint32_t index_size){

    //theloume na kanoume BFS kai ap tis 2 meries kai na sugrinoume ta epomena an sumpiptoun
    uint32_t *outBarrier = NULL, *inBarrier = NULL;
    list_node *curStart = NULL,*curEnd = NULL;
    int i = 0, in = 0;


    //olo se mia loopa kai domi gia apofugi kuklwn
    curStart = bufferOut + getListHead(index,start, index_size);
    curEnd = bufferIn + getListHead(index,end, index_size);

    outBarrier = nextBarrier(curStart);
    inBarrier = nextBarrier(curEnd);


    for(i = 0; i < N; i++){
        in = isIn(outBarrier[i],inBarrier);
        if(in == FOUND){
            break;
        }
    }

}