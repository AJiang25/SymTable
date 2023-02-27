/*-------------------------------------------------------------------*/
/* symtablehash.c                                                    */
/* Author: Arnold Jiang                                              */
/*-------------------------------------------------------------------*/

#include "symtable.h"

enum {BUCKET_COUNT = 509};

struct SymTable {
    /*points to the bucket*/
    struct Bind **buckets;
    /*tracks the number of binds*/
    size_t counter;
};

struct Bind {
    /*points to a string that represents the key*/
    char *key;
    /*points to a value*/
    const void *value;
    /*points to the next bind in the linked list*/
    struct Bind *next;
};

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
    size_t i;
    SymTable_T oSymTable; 

    /*allocates memory for a new SymTable*/
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    } 

    oSymTable->buckets = calloc(BUCKET_COUNT, sizeof(struct Bind*));
    if (oSymTable->buckets == NULL) {
        free(oSymTable);
        return NULL;
    }

    /*Sets counter to 0*/
    oSymTable->counter = 0;

    return oSymTable;
}

/* frees all memory occupied by oSymTable. */
void SymTable_free(SymTable_T oSymTable) {;
    size_t i;
    struct Bind *bind;
    struct Bind *next;
    assert(oSymTable != NULL);

    for (i = 0; i < BUCKET_COUNT; i++) {
        bind = oSymTable->buckets[i];
        while (bind != NULL) {
            next = bind->next;
            free(bind->key);
            free(bind);
            bind = next;
        }
    }
    /*frees the overall SymTable after values are freed */
    free(oSymTable);
}

/* Returns the number of bindings in oSymTable.*/
size_t SymTable_getLength(SymTable_T oSymTable) {
   /*checks that oSymTable isn't NULL*/
    assert(oSymTable != NULL);
    return oSymTable->counter;
}

/* Add a binding to OsymTable with key pcKey and value *pvValue and 
returns 1 (TRUE), otherwise return 0 (FALSE) and leave oSymTable 
unchanged. Return a checked runtime error when oSymTable or pcKey 
is NULL */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* If oSymTable contains a binding with key pcKey, then 
SymTable_replace must replace the binding's value with pvValue 
and return the old value. Otherwise it must leave oSymTable 
unchanged and return NULL. */
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* return 1 (TRUE) if oSymTable contains a binding whose key 
is pcKey, and 0 (FALSE) otherwise.*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* return the value of the binding within oSymTable whose key 
is pcKey, or NULL if no such binding exists.*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* If oSymTable contains a binding with key pcKey, then 
SymTable_remove must remove that binding from oSymTable and 
return the binding's value. Otherwise the function must not 
change oSymTable and return NULL.*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);


/* must apply function *pfApply to each binding in oSymTable, 
passing pvExtra as an extra parameter. That is, the function 
must call (*pfApply)(pcKey, pvValue, pvExtra) for each pcKey/pvValue 
binding in oSymTable.*/
void SymTable_map(SymTable_T oSymTable, void (*pfApply)
(const char *pcKey, void *pvValue, void *pvExtra), 
const void *pvExtra) {
   struct Bind *current;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (current = oSymTable->buckets; current != NULL; 
             current = current->next) {
                (*pfApply)((void*)current->key, 
                (void*) current->value, (void*) pvExtra);
}
}