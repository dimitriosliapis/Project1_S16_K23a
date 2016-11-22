#include <stdlib.h>
#include "hash.h"

unsigned int hash(uint32_t x) {

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

ht_Node **createHashtable(uint32_t size) {

    int i = 0;
    ht_Node **hashTable = NULL;
    hashTable = malloc(sizeof(ht_Node) * size);
    if (hashTable == NULL)
        return NULL;
    else {
        for (i = 0; i < size; i++)
            hashTable[i] = NULL;
        return hashTable;
    }
}

int search(ht_Node **hashTable, uint32_t id, uint32_t size) {

    int offset = hash(id) % size;
    ht_Node *bucket = NULL;

    if (hashTable == NULL)
        return NOT_FOUND;

    bucket = hashTable[offset];
    while (bucket != NULL) {

        if (bucket->id == id)
            return FOUND;
        bucket = bucket->next;
    }

    return NOT_FOUND;
}

void insert(ht_Node **hashTable, uint32_t id, uint32_t size) {

    int offset = hash(id) % size;
    ht_Node *bucket = hashTable[offset];

    if (bucket == NULL) { // this bucket doesn't exist yet - create it
        ht_Node *htEntry = (ht_Node *) malloc(sizeof(ht_Node));
        htEntry->id = id;
        htEntry->next = NULL;
        hashTable[offset] = htEntry;
        return;
    }
    while (bucket->next != NULL) { // walk the list
        bucket = bucket->next;
    }
    ht_Node *htEntry = (ht_Node *) malloc(sizeof(ht_Node));
    htEntry->id = id;
    htEntry->next = NULL;
    bucket->next = htEntry;

}

void delete(ht_Node **hashTable,uint32_t size) {

    int i = 0;
    ht_Node *bucket = NULL;
    ht_Node *prev = NULL;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++) {
        bucket = hashTable[i];
        while (bucket != NULL) {
            prev = bucket;
            bucket = bucket->next;
            free(prev);
        }
    }

    free(hashTable);
}
