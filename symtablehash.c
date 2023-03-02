/*-------------------------------------------------------------------*/
/* symtablehash.c                                                    */
/* Author: Arnold Jiang                                              */
/*-------------------------------------------------------------------*/

#include "symtable.h"

/*defines FALSE (0) and TRUE (1)*/
enum {FALSE, TRUE};

/*global variable to store total bucket counts as a size_t array*/
static const size_t auBucketCounts[] = {509, 1021, 2039, 
    4093, 8191, 16381, 32749, 65521};

/* A SymTable structure is a "manager" structure that points
to a "bucket" and contains a counter that maintains the number
of binds*/
struct SymTable {
    /*points to the bucket*/
    struct Bind **buckets;
    /*tracks the number of binds*/
    size_t counter;
};

/* A value and unique key is stored in a bind. Binds are linked
 to form a list*/
struct Bind {
    /*points to a string that represents the key*/
    char *key;
    /*points to a value*/
    const void *value;
    /*points to the next bind in the linked list*/
    struct Bind *next;
};

static size_t bucketSize(SymTable_T oSymTable) {
    size_t i = 0;
    /*last array index in auBucketCounts[]*/
    int last = 7;
    /* stores the initial Bucket Counts when i is 0*/
    size_t numBucketCounts = 
            sizeof(auBucketCounts)/sizeof(auBucketCounts[i]);

    /* handles the case in which auBucketCounts is at a max*/
    if (numBucketCounts == auBucketCounts[last]) {
        return numBucketCounts;
    }

    /* if the number of binds is less than the number of Buckets,
    incrememnt i */
    /*510 binds > 509 buckets*/
    if (oSymTable->counter > numBucketCounts) {
        i++;
        oSymTable->buckets = 
            realloc((void*)auBucketCounts[i], sizeof(struct Bind*));
        numBucketCounts = 
            sizeof(auBucketCounts)/sizeof(auBucketCounts[i]);
    }
    return numBucketCounts;
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;
   assert(pcKey != NULL);
   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable; 

    /*allocates memory for a new SymTable*/
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    } 

    /*allocates memory for the buckets in the SymTable*/
    oSymTable->buckets = calloc
        (bucketSize(oSymTable), sizeof(struct Bind*));
    if (oSymTable->buckets == NULL) {
        free(oSymTable);
        return NULL;
    }

    /*Sets counter to 0*/
    oSymTable->counter = 0;

    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    size_t i;
    struct Bind *bind;
    struct Bind *next;
    assert(oSymTable != NULL);

    /*iterates through every bucket, goes through every node 
    in each bucket, and removes the key & node*/
    for (i = 0; i < bucketSize(oSymTable); i++) {
        bind = oSymTable->buckets[i];
        while (bind != NULL) {
            next = bind->next;
            free(bind->key);
            free(bind);
            bind = next;
        }
    }

    /*frees the linked list array: buckets*/
    free(oSymTable->buckets);

    /*frees the overall SymTable after values are freed */
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
   /*checks that oSymTable isn't NULL*/
    assert(oSymTable != NULL);
    return oSymTable->counter;
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        size_t numBucketCounts;
        struct Bind *newBind;
        char *copy;
        size_t hash;
        assert(oSymTable != NULL);
        assert(pcKey != NULL);

        hash = SymTable_hash(pcKey, bucketSize(oSymTable));

        /* checks if pcKey exists already in SymTable*/
        if (SymTable_contains(oSymTable, pcKey)) {
            return FALSE;
        }

        /*Makes a Defensive Copy of the string that pcKey points to &
        stores the address of that copy in a new binding*/
        copy = malloc(strlen(pcKey) + 1);
        if (copy == NULL) {
            return FALSE;
        }
        strcpy(copy, pcKey);

        /*allocates memory for the newBind*/
        newBind = (struct Bind*)malloc(sizeof(struct Bind));
        if (newBind == NULL) {
            free(copy);
            return FALSE;
        }

        /*assigns key and value*/
        newBind->key = (char*)copy;
        newBind->value = (void*)pvValue;
        
        /*inserts the newBind into the SymTable*/
        newBind->next = oSymTable->buckets[hash];
        oSymTable->buckets[hash] = newBind;
        oSymTable->counter++;
        return TRUE;
    }

void *SymTable_replace(SymTable_T oSymTable, 
    const char *pcKey, const void *pvValue) {
        size_t hash;
        struct Bind *tmp;
        void* val;
        assert(oSymTable != NULL);
        assert(pcKey != NULL);
        val = NULL;
        hash = SymTable_hash(pcKey, bucketSize(oSymTable));
        
        /* checks if oSymTable contains the key */
        if (SymTable_contains(oSymTable, pcKey) != 1) {
            return NULL;
        }

        /* replaces the value with a given value */
        for (tmp = oSymTable->buckets[hash]; 
            tmp != NULL; tmp = tmp->next){
            if (strcmp(pcKey, tmp->key) == 0) {
                val = (void*)tmp->value;
                tmp->value = pvValue;
            }
        }
        return val;
    }

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t hash;
    struct Bind *tmp;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hash = SymTable_hash(pcKey, bucketSize(oSymTable));
    for (tmp = oSymTable->buckets[hash]; tmp != NULL; tmp = tmp->next){
        if (strcmp(pcKey, tmp->key) == 0) 
            return TRUE;
    }
    return FALSE;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t hash;
    struct Bind *tmp;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hash = SymTable_hash(pcKey, bucketSize(oSymTable));

    if (SymTable_contains(oSymTable, pcKey) == 0) {
        return NULL;
    }
    for (tmp = oSymTable->buckets[hash]; tmp != NULL; tmp = tmp->next){
        if (strcmp(pcKey, tmp->key) == 0) 
            return (void*)tmp->value;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t hash;
    struct Bind *tmp;
    struct Bind *before;
    void *val;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, bucketSize(oSymTable));
    val = NULL;
    before = NULL;

    /* checks if oSymTable contains the key */
    if (SymTable_contains(oSymTable, pcKey)) {
        for (tmp = oSymTable->buckets[hash]; 
        tmp != NULL; tmp = tmp->next){
            if (strcmp(pcKey, tmp->key) == 0) 
                break;
            }

        /* skips to the bind */
        for (tmp = oSymTable->buckets[hash]; 
        tmp!= NULL && strcmp(tmp->key, pcKey); 
            before = tmp, tmp = tmp->next);

        /*handles first bind case*/
        if (before == NULL) {
            val = (void*)tmp->value;
            oSymTable->buckets[hash] = tmp->next;
        }
        /*handles other bind cases*/
        else {
            val = (void*)tmp->value;
            before->next = tmp->next;
        }
        /* keys the key, tmp, decrements counter, returns val*/
        free(tmp->key);
        free(tmp);
        oSymTable->counter--;
        return val;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable, void (*pfApply)
    (const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra) {
    size_t i;
    struct Bind *current;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (i = 0; i < bucketSize(oSymTable); i++) {
        current = oSymTable->buckets[i];
        while (current != NULL) {
            (*pfApply)((void*)current->key, 
                (void*) current->value, (void*) pvExtra);
            current = current->next;
        }   
    }
}