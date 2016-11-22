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

void insert(ht_Node *hashTable, uint32_t id, uint32_t size) {

    int i = 0;
    int offset = id % size;
    int prev_size = hashTable[offset].size;

    if (hashTable[offset].bucket == NULL) {    // this bucket doesn't exist yet - create it and insert id
        hashTable[offset].bucket = malloc(HT_N * sizeof(uint32_t));
        hashTable[offset].bucket[0] = id;
        for (i = 1; i < HT_N; i++) {
            hashTable[offset].bucket[i] = DEFAULT;
        }
        hashTable[offset].size = HT_N;
        return;

    } else {

        while (i < prev_size) { // find empty cell in bucket and insert id
            if (hashTable[offset].bucket[i] == DEFAULT) {
                hashTable[offset].bucket[i] = id;
                return;
            }
            i++;
        }

        // duplicate bucket and insert id
        hashTable[offset].bucket = realloc(hashTable[offset].bucket, prev_size * 2 * sizeof(uint32_t));
        hashTable[offset].size *= 2;
        hashTable[offset].bucket[prev_size] = id;
        for (i = prev_size + 1; i < hashTable[offset].size; i++) {
            hashTable[offset].bucket[i] = DEFAULT;
        }
    }
}

void reinitialize(ht_Node *hashTable, uint32_t size) {

    int i = 0, j = 0;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++) {
        if (hashTable[i].bucket != NULL) {
            for (j = 0; j < hashTable[i].size; j++) {
                if (hashTable[i].bucket[j] == DEFAULT)
                    break;
                hashTable[i].bucket[j] = DEFAULT;
            }
        }
    }
}

void delete(ht_Node *hashTable, uint32_t size) {

    int i = 0;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++) {
        free(hashTable[i].bucket);
    }

    free(hashTable);
}
