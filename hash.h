#include <stdint.h>

#define HT_BIG 148201
#define HT_SMALL 157
#define HT_N 3

#define THREAD_POOL_SIZE 2


#define DEFAULT 16777216

#define FOUND 1
#define NOT_FOUND -1

int hash( int a);

typedef struct htEntry_t{
    int id;
    int version[THREAD_POOL_SIZE+1];
}ht_Entry;

typedef struct htNode_t {
    ht_Entry *bucket;
    int size;
} ht_Node;

ht_Node *createHashtable(int);

int search(ht_Node *, int, int, int, int);

void insert(ht_Node *, int, int, int, int);

void delete(ht_Node *, int);
