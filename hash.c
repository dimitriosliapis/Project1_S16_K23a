#include <stdlib.h>
#include "hash.h"

unsigned int hash(uint32_t x) {

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

ht_Node *createHashtable(uint32_t size) {

    int i = 0, j = 0;
    ht_Node *hashTable = NULL;
    hashTable = malloc(sizeof(ht_Node) * size);
    if (hashTable == NULL)
        return NULL;
    else {
        for(i = 0; i < size; i++){
            hashTable[i].ids = malloc(HT_N * sizeof(uint32_t));
            for(j = 0; j < HT_N; j++){
                hashTable[i].ids[j] = DEFAULT;
            }
        }
        hashTable[i].size = HT_N;
        return hashTable;
    }
}

int search(ht_Node *hashTable, uint32_t id, uint32_t size) {

    int offset = hash(id) % size;
    int i = 0;
    uint32_t *bucket = NULL;

    if (hashTable == NULL)
        return NOT_FOUND;

    bucket = hashTable[offset].ids;
    i = 0;
    while (i < hashTable[offset].size) {

        if (bucket[i] == id)/*while (bucket != NULL) {
            prev = bucket;
            bucket = bucket->next;
            free(prev);
        }*/
            return FOUND;
        i++;
    }

    return NOT_FOUND;
}

void insert(ht_Node *hashTable, uint32_t id, uint32_t size) {

    int offset = hash(id) % size;
    unsigned int s = hashTable[offset].size;
    int i = 0;

    if(hashTable[offset].ids[s-1] != DEFAULT){
        s *= 2;
        hashTable[offset].ids = realloc(hashTable[offset].ids,s * sizeof(uint32_t));
        hashTable[offset].ids[hashTable[offset].size] = id;
        hashTable[offset].size = s;
        return;
    }

    for(i = 0; i < hashTable[offset].size; i++){
        if(hashTable[offset].ids[i] == DEFAULT) hashTable[offset].ids[i] = id;
    }

    /*if (bucket == NULL) { // this bucket doesn't exist yet - create it
        ht_Node *htEntry = (ht_Node *) malloc(sizeof(ht_Node));
        htEntry->ids[0] = id;
        //htEntry->next = NULL;
        hashTable[offset] = htEntry;
        return;
    }
    while (bucket->next != NULL) { // walk the list
        bucket = bucket->next;
    }
    ht_Node *htEntry = (ht_Node *) malloc(sizeof(ht_Node));
    htEntry->id = id;
    htEntry->next = NULL;
    bucket->next = htEntry;*/

}

void delete(ht_Node *hashTable,uint32_t size) {

    int i = 0;
    uint32_t *bucket = NULL;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++) {
        bucket = hashTable[i].ids;
        free(bucket);
        /*while (bucket != NULL) {
            prev = bucket;
            bucket = bucket->next;
            free(prev);
        }*/
    }

    free(hashTable);
}
