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

/* returns a new SymTable object that is created with no bindings
, or NULL if insufficient memory is available.*/
SymTable_T SymTable_new(void);

/* frees all memory occupied by oSymTable. */
void SymTable_free(SymTable_T oSymTable);

/* Returns a size_t of the number of bindings in oSymTable.*/
size_t SymTable_getLength(SymTable_T oSymTable);

/* Adds a binding to oSymTable with key pcKey and value *pvValue and 
returns 1 (TRUE), otherwise return 0 (FALSE) and leaves oSymTable 
unchanged. Return a checked runtime error when oSymTable or pcKey 
is NULL */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* If oSymTable contains a binding with key pcKey, then 
the function replaces the binding's value with pvValue 
and returns the old value. Otherwise it oSymTable is
unchanged and returns NULL. */
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* return 1 (TRUE) if oSymTable contains a binding whose key 
is pcKey, and 0 (FALSE) otherwise.*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* returns the value of the binding within oSymTable whose key 
is pcKey, or NULL if no such binding exists.*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* If oSymTable contains a binding with key pcKey, then 
SymTable_remove removes that binding from oSymTable and 
return the binding's value. Otherwise the function doesn't 
change oSymTable and returns NULL.*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Apply function *pfApply to each binding in oSymTable, 
passing pvExtra as an extra parameter. That is, the function 
must call (*pfApply)(pcKey, pvValue, pvExtra) for each pcKey/pvValue 
binding in oSymTable.*/
void SymTable_map(SymTable_T oSymTable, void (*pfApply)
(const char *pcKey, void *pvValue, void *pvExtra), 
const void *pvExtra);

#endif