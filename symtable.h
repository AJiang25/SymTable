/*-------------------------------------------------------------------*/
/* symtable.h                                                        */
/* Author: Arnold Jiang                                              */
/*-------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

/* A SymTable_T object is a an unordered collection of bindings that
hold a unique key and a value*/

typedef struct SymTable *SymTable_T;

/* */
/*static size_t SymTable_hash(const char *pcKey, size_t uBucketCount);
*/

/* returns a new SymTable object that contains no bindings
, or NULL if insufficient memory is available.*/
SymTable_T SymTable_new(void);

/* frees all memory occupied by oSymTable. */
void SymTable_free(SymTable_T oSymTable);

/* Returns the number of bindings in oSymTable.*/
size_t SymTable_getLength(SymTable_T oSymTable);

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
const void *pvExtra);

#endif