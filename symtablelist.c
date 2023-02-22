/*-------------------------------------------------------------------*/
/* symtablelist.c                                                    */
/* Author: Arnold Jiang                                              */
/*-------------------------------------------------------------------*/

#include "symtable.h"

/* A SymTable structure is a "manager" structure that points
to the first Bind and contains a counter that maintains the number
of binds*/
struct SymTable {
    struct Bind *first;
    size_t counter;
};

/* A value and unique key is stored in a bind. Binds are linked
 to form a list*/
struct Bind {
    char *key;
    const void *value;
    struct Bind *next;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;

    /*allocates memory for a new SymTable*/
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }   

    /*Sets first to NULL, the key & value of first to NULL
    , and counter to 0*/
    oSymTable->first = NULL;
    oSymTable->first->key = NULL;
    oSymTable->first->value = NULL;
    oSymTable->counter = 0; 

    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    struct Bind *bind;
    struct Bind *next;
    assert(oSymTable != NULL);

    for (bind = oSymTable->first; bind != NULL; bind = next) {
        /* sets next to the bind's next target */
        next = bind->next;

        /*frees the keys & values */
        free(bind->key);
        free((void*)(bind->value));

        /* frees the bind */
        free(bind);
    }

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
        size_t *length;
        assert(oSymTable != NULL);
        assert(pcKey != NULL);
        assert(pvValue != NULL);

        /*Makes a Defensive Copy of the string that pcKey points to &
        stores the address of that copy in a new binding*/
        length = (size_t*)malloc(sizeof(size_t*));
        length = (size_t*)(strlen(pcKey));

        copy = (char*)malloc(sizeof(length));
        strcpy(copy, pcKey);

        /*allocates memory for the newBind*/
        newBind = (Bind)malloc(sizeof(struct Bind));

        if (newBind == NULL) {
            return 0;
        }

        /*assigns key and value*/
        newBind->key = (char*)copy;
        newBind->value = (void*)pvValue;

        /*inputs the newBind at the beginning of the SymTable*/
        newBind->next = oSymTable->first;
        oSymTable->first = newBind;
        oSymTable->counter++;
        return 1;
    }

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        struct Bind *tmp;
        void* val;
        assert(oSymTable != NULL);
        assert(pcKey != NULL);
        assert(pvValue != NULL);
        
        /* checks if oSymTable contains the key */
        if (SymTable_contains(oSymTable, pcKey) != 1) {
            return NULL;
        }

        /* replaces the value with a given value */
        for (tmp = oSymTable->first; tmp!= NULL; tmp = tmp->next) {
            if (tmp->key == pcKey) {
                val = (void*)(tmp->value); 
                tmp->value = pvValue;
            }
        }
        return val;
    }

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Bind *tmp;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    for (tmp = oSymTable->first; tmp!= NULL; tmp = tmp->next) {
        if (tmp->key == pcKey) {
            return 1;
        }
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Bind *tmp;
    void *item;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* gets the value */
    for (tmp = oSymTable->first; tmp!= NULL; tmp = tmp-> next) {
        item = (void*)(oSymTable->first->value);
        if (tmp->key == pcKey) {
            return item;
        }
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct Bind *tmp;
    size_t val;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* checks if oSymTable contains the key */
    if (SymTable_contains(oSymTable, pcKey) != 1) {
        return NULL;
    }
    /* removes the value with a given value */
    for (tmp = oSymTable-> first; tmp!= NULL; tmp = tmp-> next) {
        if (tmp->key == pcKey) {
            val = (size_t)tmp->value;
            free((void*)(tmp->value));
            free(tmp->key);
            tmp->next = tmp->next->next;
            free(tmp);
        }
    }
    return (void*)val;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
        struct Bind *current;

        assert(oSymTable != NULL);
        assert(pfApply != NULL);

        for (current = oSymTable->first; current != NULL; 
             current = current->next) {
                (*pfApply)((void*)current->key, 
                (void*) current->value, (void*) pvExtra);
        }

    }