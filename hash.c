#include <stdlib.h>
#include "hash.h"

int hash( int a){
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}
ht_Node *createHashtable(int size) {

    int i = 0;
    ht_Node *hashTable = NULL;
    hashTable = malloc(sizeof(ht_Node) * size);
    if (hashTable == NULL)
        return NULL;
    else {
        for (i = 0; i < size; i++) {
            hashTable[i].bucket = NULL;
            hashTable[i].size = size;
        }
        return hashTable;
    }
}

int search(ht_Node *hashTable, int id, int size, int version, int thread_id) {

    int i = 0;
    int offset = hash(id) % size;
    ht_Entry *bucket = NULL;

    if (hashTable == NULL)
        return NOT_FOUND;

    bucket = hashTable[offset].bucket;
    if (bucket == NULL)
        return NOT_FOUND;

    while (i < hashTable[offset].size) {
        if ((bucket[i].id == id) && (bucket[i].version[thread_id] == version))
            return FOUND;
       /* else if(bucket[i].id == id)
            return NOT_FOUND;*/
        i++;
    }

    return NOT_FOUND;
}

void insert(ht_Node *hashTable, int id, int size, int version, int thread_id) {

    int i = 0;
    int offset = hash(id) % size;
    int prev_size = hashTable[offset].size;

    if (hashTable[offset].bucket == NULL) {    // this bucket doesn't exist yet - create it and insert id
        hashTable[offset].bucket = malloc(HT_N * sizeof(ht_Entry));
        hashTable[offset].bucket[0].id = id;
        hashTable[offset].bucket[0].version[thread_id] = version;
        hashTable[offset].bucket[1].version[thread_id] = DEFAULT;
        /*for (i = 1; i < HT_N; i++) {
            hashTable[offset].bucket[i].id = DEFAULT;
            hashTable[offset].bucket[i].version[thread_id] = DEFAULT;
        }*/
        hashTable[offset].size = HT_N;
        return;

    } else {

        while (i < prev_size) {     // find either older version of the same entry and update or empty cell and insert data
            /*if((hashTable[offset].bucket[i].id == id) && (hashTable[offset].bucket[i].version[thread_id] != version)) {
                hashTable[offset].bucket[i].version[thread_id] = version;
                return;
            }
            else if (hashTable[offset].bucket[i].id == id) {
                hashTable[offset].bucket[i].id = id;
                hashTable[offset].bucket[i].version[thread_id] = version;
                return;
            }*/
            if(hashTable[offset].bucket[i].version[thread_id] != version) {
                hashTable[offset].bucket[i].id = id;
                hashTable[offset].bucket[i].version[thread_id] = version;
                if((i+1) < prev_size) hashTable[offset].bucket[i+1].version[thread_id] = DEFAULT;

                return;
            }
            i++;
        }

/*        if(offset == 66){
            offset = 66;
        }*/
        // duplicate bucket and insert id

        hashTable[offset].bucket = realloc(hashTable[offset].bucket, prev_size * 2 * sizeof(ht_Entry));
        hashTable[offset].size *= 2;
        hashTable[offset].bucket[prev_size].id = id;
        hashTable[offset].bucket[prev_size].version[thread_id] = version;
        hashTable[offset].bucket[prev_size+1].version[thread_id] = DEFAULT;
        /*for (i = prev_size + 1; i < hashTable[offset].size; i++) {
            hashTable[offset].bucket[i].id = DEFAULT;
            hashTable[offset].bucket[i].version[thread_id] = DEFAULT;
        }*/
    }
}

void delete(ht_Node *hashTable, int size) {

    int i = 0;

    if (hashTable == NULL)
        return;

    for (i = 0; i < size; i++) {
        free(hashTable[i].bucket);
    }

    free(hashTable);
}
