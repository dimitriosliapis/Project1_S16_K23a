#include <stdint.h>

#define HT_BIG 100003
#define HT_SMALL 10007

#define FOUND 1
#define NOT_FOUND -1

typedef struct htNode_t {
    uint32_t id;
    struct htNode_t *next;
} ht_Node;

unsigned int hash(uint32_t x);

ht_Node **createHashtable(uint32_t);

int search(ht_Node**, uint32_t, uint32_t);

void insert(ht_Node**, uint32_t, uint32_t);

void delete(ht_Node**, uint32_t);
