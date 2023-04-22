/* 
 * set.c - CS50 'set' module
 *
 * see set.h for more information.
 *
 * Trung Nguyen, Dartmouth CS50. Lab 3 - Spring 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct setnode {
  char* key;                  // pointer to key for this item
  void* item;                 // pointer to data for this item
  struct setnode *next;       // link to next node
} setnode_t;

/**************** global types ****************/
typedef struct set {
  struct setnode *head;       // pointer to the head of the set
} set_t;

/**************** local functions ****************/
/* not visible outside this file */
static setnode_t* setnode_new(const char* key, void* item);

/**************** setnode_new ****************/
/* Allocate and initialize a setnode */
static setnode_t*  // not visible outside this file
setnode_new(const char* key, void* item)
{
  setnode_t* node = mem_malloc(sizeof(setnode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  }

  // allocating memory for copy of the string
  char* keyString = mem_malloc(strlen(key) * sizeof(char) + 1);
  if (keyString == NULL) {
    mem_free(node);
    return NULL;
  }

  memcpy(keyString, key, strlen(key) + 1);
  node->key = keyString;
  node->item = item;
  node->next = NULL;
  return node;
}

/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** set_new() ****************/
/* see set.h for description */
set_t*
set_new(void)
{
  set_t* set = mem_malloc(sizeof(set_t));
  if (set == NULL) {
    return NULL;              // error allocating set
  } else {
    // initialize contents of set structure
    set->head = NULL;
    return set;
  }
}

/**************** set_insert ****************/
/* see set.h for description */
bool 
set_insert(set_t* set, const char* key, void* item)
{
  if (set == NULL || key == NULL || item == NULL) {     // if any parameters are null
    return false;
  }
  for (setnode_t* node = set->head; node != NULL; node = node->next) {  
    if (strcmp(key, node->key) == 0) {
      return false;
    }
  }

  setnode_t* new = setnode_new(key, item);              // allocate a new node
  if (new == NULL || new->item == NULL || new->key == NULL) {
    mem_free(new);
    return false;                                       // error allocating a new node
  }

  // insert to the head
  new->next = set->head;
  set->head = new;

#ifdef MEMTEST
  mem_report(stdout, "After set_insert");
#endif

  return true;
}

/**************** set_find ****************/
/* see set.h for description */
void* 
set_find(set_t* set, const char* key)
{
  if (set == NULL || key == NULL) {
    return NULL;
  }

  // iterate through the set
  for (setnode_t* node = set->head; node != NULL; node = node->next) {
    // if match
    if (strcmp(node->key, key) == 0) {
      return node->item;
    }
  }

  // if after iterating through the whole set, and no key found
  return NULL;
}

/**************** set_print ****************/
/* see set.h for description */
void 
set_print(set_t* set, FILE* fp, 
                  void (*itemprint)(FILE* fp, const char* key, void* item) )
{
  if (fp == NULL) {           // if fp is null
    return;
  }
  if (set == NULL) {          // if set is null
    fputs("(null)", fp);
    return;
  }

  fputc('{', fp);
  for (setnode_t* node = set->head; node != NULL; node = node->next) {
    // print this node
    if (itemprint != NULL) {
      (*itemprint)(fp, node->key, node->item);
      if (node->next != NULL) {
        fputc(',', fp);
      }
    }
  }
  fputc('}', fp);
}

/**************** set_iterate ****************/
/* see set.h for description */
void 
set_iterate(set_t* set, void* arg,
                  void (*itemfunc)(void* arg, const char* key, void* item) )
{
  // if set or itemfunc is null
  if (set == NULL || itemfunc == NULL) {
    return;
  }

  for (setnode_t* node = set->head; node != NULL; node = node->next) {
    // call itemfunc with arg, on each item, with (arg, key, item)
    (*itemfunc)(arg, node->key, node->item);
  }
}

/**************** set_delete ****************/
/* see set.h for description */
void 
set_delete(set_t* set, void (*itemdelete)(void* item) )
{
  // set is null
  if (set == NULL) {
    return;
  }

  for (setnode_t* node = set->head; node != NULL; ) {
    mem_free(node->key);              // free the memory of the (copy) of the key
    if (itemdelete != NULL) {
      (*itemdelete)(node->item);      // delete node's item
    }
    setnode_t* next = node->next;     // remember what come next
    mem_free(node);                   // free the node 
    node = next;                      // move on
  }

  mem_free(set);
#ifdef MEMTEST
  mem_report(stdout, "End of set_delete");
#endif
}