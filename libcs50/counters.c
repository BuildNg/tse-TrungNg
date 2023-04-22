/* 
 * counters.c - CS50 'counters' module
 *
 * see set.h for more information.
 *
 * Trung Nguyen, Dartmouth CS50. Lab 3 - Spring 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct countersnode {
  int key;                       
  int counter;                   
  struct countersnode *next;      // pointer to next node
} countersnode_t;

/**************** global types ****************/
typedef struct counters {
  struct countersnode *head;       // pointer to the head of the counter
} counters_t;

/**************** local functions ****************/
/* not visible outside this file */
static countersnode_t* countersnode_new(const int key, const int count);

/**************** countersnode_new ****************/
/* Allocate and initialize a countersnode */
static countersnode_t*
countersnode_new(const int key, const int count)
{
  countersnode_t* node = mem_malloc(sizeof(countersnode_t));
  if (node == NULL) {
    return NULL;
  } else {
    node->counter = count;
    node->key = key;
    node->next = NULL;
    return node;
  }
}

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** counters_new() ****************/
/* see counters.h for description */
counters_t*
counters_new(void)
{
  counters_t* ctrs = mem_malloc(sizeof(counters_t));
  if (ctrs == NULL) {
    return NULL;          // error allocating memory for counters
  } else {
    // initializing counters data structure
    ctrs->head = NULL;
    return ctrs;
  }
}

/**************** counters_add() ****************/
/* see counters.h for description */
int 
counters_add(counters_t* ctrs, const int key)
{
  if (ctrs == NULL || key < 0) {
    return 0;
  }
  // check whether the key already exists
  for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
    if (node->key == key) {
      return ++node->counter;
    }
  }
  // if the key does not exist
  countersnode_t* new = countersnode_new(key, 1); 
  if (new == NULL) {
    return 0;           // if fail to allocate memory
  }
  new->next = ctrs->head;
  ctrs->head = new;
  return new->counter;

#ifdef MEMTEST
  mem_report(stdout, "After counters_add");
#endif
}

/**************** counters_get() ****************/
/* see counters.h for description */
int 
counters_get(counters_t* ctrs, const int key)
{
  if (ctrs == NULL) {
    return 0;
  }
  // look through the counters to find the key
  for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
    if (node->key == key) {
      return node->counter;
    }
  }

  // if no key matches
  return 0;
}

/**************** counters_set() ****************/
/* see counters.h for description */
bool 
counters_set(counters_t* ctrs, const int key, const int count)
{
  if (ctrs == NULL || key < 0 || count < 0) {
    return false;
  }

  // look through the counters to check if such a key exists
  for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
    if (node->key == key) {
      node->counter = count;
      return true;
    }
  }

  // if no such key exists, initialize it
  countersnode_t* new = countersnode_new(key, count);
  if (new == NULL) {
    return false;       // fail to allocate more memories (out of memory)
  }
  new->next = ctrs->head;
  ctrs->head = new;

  #ifdef MEMTEST
  mem_report(stdout, "After setting a new node by counters_set");
  #endif
  return true;
}

/**************** counters_print() ****************/
/* see counters.h for description */
void counters_print(counters_t* ctrs, FILE* fp)
{
  if (fp == NULL) {
    return;
  }
  if (ctrs == NULL) {
    fprintf(fp, "(null)");
    return;
  }
  
  fprintf(fp, "{");
  // iterate through the counters
  for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
    fprintf(fp, "%d= %d", node->key, node->counter);
    if (node->next != NULL) {
      fprintf(fp, ", ");
    }
  }
  fprintf(fp, "}");
}

/**************** counters_iterate() ****************/
/* see counters.h for description */
void 
counters_iterate(counters_t* ctrs, void* arg, 
                      void (*itemfunc)(void* arg, 
                                       const int key, const int count))
{
  if (ctrs == NULL || itemfunc == NULL) {
    return;
  }
  for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
    (*itemfunc)(arg, node->key, node->counter);
  }
}

/**************** counters_delete() ****************/
/* see counters.h for description */
void 
counters_delete(counters_t* ctrs)
{
  if (ctrs == NULL) {
    return;
  }
  for (countersnode_t* node = ctrs->head; node != NULL; ) {
    countersnode_t* next = node->next;
    mem_free(node);
    node = next;
  }
  mem_free(ctrs);

  #ifdef MEMTEST
  mem_report(stdout, "End of counters delete");
  #endif
}
