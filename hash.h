#include <stdint.h>

#define HT_BIG 148201
#define HT_SMALL 67
#define HT_N 7

#define DEFAULT 16777216

#define FOUND 1
#define NOT_FOUND -1

typedef struct htEntry_t{
    uint32_t id;
    int steps;
    uint32_t version;
}ht_Entry;

typedef struct htNode_t {
    ht_Entry *bucket;
    uint32_t size;
} ht_Node;

unsigned int hash(uint32_t x);

ht_Node *createHashtable(uint32_t);

int search(ht_Node *, uint32_t, uint32_t, uint32_t);

void insert(ht_Node *, uint32_t, uint32_t, uint32_t);

void bfs_insert(ht_Node *, uint32_t, uint32_t, int, uint32_t);

int num_steps(ht_Node *, uint32_t, uint32_t, uint32_t);

void delete(ht_Node *, uint32_t);
