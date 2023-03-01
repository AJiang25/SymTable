/*-------------------------------------------------------------------*/
/* symtablehash.c                                                    */
/* Author: Arnold Jiang                                              */
/*-------------------------------------------------------------------*/

#include "symtable.h"

/*The number of hash buckets used in the symtable*/
enum {BUCKET_COUNT = 509};

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
    oSymTable->buckets = calloc(BUCKET_COUNT, sizeof(struct Bind*));
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
    for (i = 0; i < BUCKET_COUNT; i++) {
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
        struct Bind *newBind;
        char *copy;
        size_t hash;
        assert(oSymTable != NULL);
        assert(pcKey != NULL);

        hash = SymTable_hash(pcKey, BUCKET_COUNT);

        /* checks if pcKey exists already in SymTable*/
        if (SymTable_contains(oSymTable, pcKey)) {
            return 0;
        }

        /*Makes a Defensive Copy of the string that pcKey points to &
        stores the address of that copy in a new binding*/
        copy = malloc(strlen(pcKey) + 1);
        if (copy == NULL) {
            return 0;
        }
        strcpy(copy, pcKey);

        /*allocates memory for the newBind*/
        newBind = (struct Bind*)malloc(sizeof(struct Bind));
        if (newBind == NULL) {
            free(copy);
            return 0;
        }

        /*assigns key and value*/
        newBind->key = (char*)copy;
        newBind->value = (void*)pvValue;
        
        /*inserts the newBind into the SymTable*/
        newBind->next = oSymTable->buckets[hash];
        oSymTable->buckets[hash] = newBind;
        oSymTable->counter++;
        return 1;
    }

void *SymTable_replace(SymTable_T oSymTable, 
    const char *pcKey, const void *pvValue) {
        size_t hash;
        struct Bind *tmp;
        void* val;
        assert(oSymTable != NULL);
        assert(pcKey != NULL);
        val = NULL;
        hash = SymTable_hash(pcKey, BUCKET_COUNT);
        
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
    hash = SymTable_hash(pcKey, BUCKET_COUNT);
    for (tmp = oSymTable->buckets[hash]; tmp != NULL; tmp = tmp->next){
        if (strcmp(pcKey, tmp->key) == 0) 
            return 1;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t hash;
    struct Bind *tmp;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hash = SymTable_hash(pcKey, BUCKET_COUNT);

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

    hash = SymTable_hash(pcKey, BUCKET_COUNT);
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

    for (i = 0; i < BUCKET_COUNT; i++) {
        current = oSymTable->buckets[i];
        while (current != NULL) {
            (*pfApply)((void*)current->key, 
                (void*) current->value, (void*) pvExtra);
            current = current->next;
        }   
    }
}