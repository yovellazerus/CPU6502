#ifndef List_H
#define List_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>

// error cods
typedef enum {
    List_OK,
    List_NOMEM,
    List_INVALID_INPUT,
    List_UNSORTABLE,
    List_UNKOWN
} List_ERROR;

// List is an ATD
typedef struct List_t List;

// function pointer types for the management of memory and compering elements
typedef void* (*List_copyFunction)(void*);
typedef void (*List_destroyFunction)(void*);
typedef void (*List_printFunction)(void*);
typedef int (*List_comperFunction)(void*, void*); 



// memory management
List* List_create(List_copyFunction copy, List_destroyFunction destroy, List_printFunction print, List_comperFunction comper);
void List_destroy(List* list);
List* List_copy(List* list);

// get's 
size_t List_getSize(List* list);
bool List_empty(List* list);
void* List_at(List* list, int index);

// standard functions 
int List_pushFront(List* list, void* item);
int List_pushBack(List* list, void* item);
void* List_popFront(List* list);
void* List_popBack(List* list);
int List_insert(List* list, int index, void* item);
void* List_extract(List* list, int index);

// exotic functions 
int List_sort(List* list);
int List_comper(List* a, List* b);
int List_clear(List* list);

// for debug
void List_print(List* list);

#endif // List_H