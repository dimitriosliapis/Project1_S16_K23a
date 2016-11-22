#define HT_SIZE 131072

#define FOUND 1
#define NOT_FOUND -1

typedef struct htNode_t {
    uint32_t id;
    struct htNode_t *next;
} ht_Node;

unsigned int hash(uint32_t x);

ht_Node **createHashtable();

int search(ht_Node **hashTable, uint32_t id);

void insert(ht_Node **hashTable, uint32_t id);

void delete(ht_Node **hashTable);
