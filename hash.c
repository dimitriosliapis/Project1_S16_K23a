#include <stdlib.h>
#include "hash.h"

unsigned int hash(uint32_t x) {

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

ht_Node *createHashtable(uint32_t size) {

    int i = 0;
    ht_Node *hashTable = NULL;
    hashTable = malloc(sizeof(ht_Node) * size);
    if (hashTable == NULL)
        return NULL;
    else {
        for (i = 0; i < size; i++) {
            hashTable[i].bucket = NULL;
//            hashTable[i].bucket = malloc(HT_N * sizeof(uint32_t));
//            for (j = 0; j < HT_N; j++) {
//                hashTable[i].bucket[j] = DEFAULT;
//            }
            hashTable[i].size = 0;
        }
        return hashTable;
    }
}

int search(ht_Node *hashTable, uint32_t id, uint32_t size) {

    int i = 0;
    int offset = id % size;
    uint32_t *bucket = NULL;

    if (hashTable == NULL)
        return NOT_FOUND;

    bucket = hashTable[offset].bucket;
    if (bucket == NULL)
        return NOT_FOUND;

    while (i < hashTable[offset].size) {

        if (bucket[i] == id)
            return FOUND;
        i++;
    }

    return NOT_FOUND;
}

void insert(ht_Node **hashTable, uint32_t id, uint32_t size) {

    int i = 0;
    int offset = id % size;

    if ((*hashTable)[offset].bucket == NULL) {    // this bucket doesn't exist yet - create it and insert id
        (*hashTable)[offset].bucket = malloc(HT_N * sizeof(uint32_t));
        (*hashTable)[offset].bucket[0] = id;
        for (i = 1; i < HT_N; i++) {
            (*hashTable)[offset].bucket[i] = DEFAULT;
        }
        (*hashTable)[offset].size = HT_N;
        return;
    }
    i = 0;

    int prev_size = (*hashTable)[offset].size;  // find empty cell in bucket and insert id
    while (i < prev_size) {
        if ((*hashTable)[offset].bucket[i] == DEFAULT) {
            (*hashTable)[offset].bucket[i] = id;
            return;
        }
        i++;
    }

    // duplicate bucket and insert id
    (*hashTable)[offset].bucket = realloc((*hashTable)[offset].bucket, prev_size * 2 * sizeof(uint32_t));
    (*hashTable)[offset].size *= 2;
    (*hashTable)[offset].bucket[prev_size] = id;
    for (i = prev_size + 1; i < (*hashTable)[offset].size; i++) {
        (*hashTable)[offset].bucket[i] = DEFAULT;
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

void delete(ht_Node *hashTable, uint32_t size) {

    int i = 0;
    uint32_t *bucket = NULL;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++) {
        bucket = hashTable[i].bucket;
        free(bucket);
        /*while (bucket != NULL) {
            prev = bucket;
            bucket = bucket->next;
            free(prev);
        }*/
    }

    free(hashTable);
}
