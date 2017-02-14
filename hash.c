#include <stdlib.h>
#include "hash.h"

// Create hash table
ht_Node *createHashtable(uint32_t size) {

    uint32_t i = 0;
    ht_Node *hashTable = NULL;

    hashTable = malloc(sizeof(ht_Node) * size);

    if (hashTable == NULL)
        return NULL;
    else {
        for (i = 0; i < size; i++) {
            hashTable[i].bucket = NULL;
            hashTable[i].size = HT_N;
        }
        return hashTable;
    }
}

// Search hash table given a node and the version
int search(ht_Node *hashTable, uint32_t id, uint32_t size, uint32_t version) {

    uint32_t i = 0;
    uint32_t offset = id % size;
    ht_Entry *bucket = NULL;

    if (hashTable == NULL)
        return NOT_FOUND;

    bucket = hashTable[offset].bucket;
    if (bucket == NULL)
        return NOT_FOUND;

    while (i < hashTable[offset].size) {
        if ((bucket[i].id == id) && (bucket[i].version == version))
            return FOUND;
        if(bucket[i].id == id)
            return NOT_FOUND;
        i++;
    }

    return NOT_FOUND;
}

// Insert node in hash table with current version
void insert(ht_Node *hashTable, uint32_t id, uint32_t size, uint32_t version) {

    uint32_t i = 0;
    uint32_t offset = id % size;
    uint32_t prev_size = hashTable[offset].size;

    if (hashTable[offset].bucket == NULL) {    // this bucket doesn't exist yet - create it and insert id
        hashTable[offset].bucket = malloc(HT_N * sizeof(ht_Entry));
        hashTable[offset].bucket[0].id = id;
        hashTable[offset].bucket[0].version = version;
        for (i = 1; i < HT_N; i++) {
            hashTable[offset].bucket[i].id = DEFAULT;
            hashTable[offset].bucket[i].version = DEFAULT;
        }
        hashTable[offset].size = HT_N;
        return;

    } else {
        // find either older version of the same entry and update or empty cell and insert data
        while (i < prev_size) {
            if(hashTable[offset].bucket[i].id == id) {
                hashTable[offset].bucket[i].version = version;
                return;
            }
            else if (hashTable[offset].bucket[i].id == DEFAULT) {
                hashTable[offset].bucket[i].id = id;
                hashTable[offset].bucket[i].version = version;
                return;
            }
            i++;
        }

        // double bucket and insert id
        hashTable[offset].bucket = realloc(hashTable[offset].bucket, prev_size * 2 * sizeof(ht_Entry));
        hashTable[offset].size *= 2;
        hashTable[offset].bucket[prev_size].id = id;
        hashTable[offset].bucket[prev_size].version = version;
        for (i = prev_size + 1; i < hashTable[offset].size; i++) {
            hashTable[offset].bucket[i].id = DEFAULT;
            hashTable[offset].bucket[i].version = DEFAULT;
        }
    }
}

// Free hash table
void delete(ht_Node *hashTable, uint32_t size) {

    uint32_t i = 0;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++)
        free(hashTable[i].bucket);

    free(hashTable);
}
