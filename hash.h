#include <stdint.h>

#define HT_BIG 328519
#define HT_SMALL 97
#define HT_N 101

#define DEFAULT 16777216

#define FOUND 1
#define NOT_FOUND -1

typedef struct bucket_t{
    uint32_t id;
    uint32_t version;
}node_bucket;

typedef struct htNode_t {
    node_bucket *bucket;
    uint32_t size;
} ht_Node;

unsigned int hash(uint32_t x);

ht_Node *createHashtable(uint32_t);

int search(ht_Node *, uint32_t, uint32_t, uint32_t);

void insert(ht_Node *, uint32_t, uint32_t, uint32_t);

void delete(ht_Node *, uint32_t);
