/* 
 * hashtable.c - CS50 'hashtable' module
 *
 * see hashtable.h for more information.
 *
 * Trung Nguyen, Dartmouth CS50. Lab 3 - Spring 2023
 */
/* 
 * hashtable.h - header file for CS50 hashtable module
 *
 * A *hashtable* is a set of (key,item) pairs.  It acts just like a set, 
 * but is far more efficient for large collections.
 *
 * David Kotz, April 2016, 2017, 2019, 2021
 * updated by Xia Zhou, July 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "hashtable.h"
#include "hash.h"
#include "set.h"

/**************** file-local global variables ****************/
/* none */

/**************** global types ****************/
typedef struct hashtable {
  int slots;         // number of slots in the table
  set_t** table;    // array of pointers to a set
} hashtable_t;  // opaque to users of the module

/**************** local functions ****************/
/* not visible outside this file */
static set_t* getset(hashtable_t* ht, const char* key);

/**************** getset ****************/
/* get the set from the hashtable by using hash function, given the key */
static set_t*
getset(hashtable_t* ht, const char* key)
{
  if (ht == NULL || key == NULL) {
    return NULL;
  }
  int mod = ht->slots;   // get the slots (which is the mod)
  unsigned long index = hash_jenkins(key, mod);     // hash function
  return ht->table[index];
}

/**************** global functions ****************/
/* that is, visible outside this file */
/* see hashtable.h for comments about exported functions */

/**************** hashtable_new ****************/
/* see hashtable.h for description */
hashtable_t* 
hashtable_new(const int num_slots)
{
  if (num_slots <= 0) {
    return NULL;
  }

  hashtable_t* ht = mem_malloc(sizeof(hashtable_t));
  if (ht == NULL) {     // if error allocating memory
    return NULL;
  }

  ht->table = mem_calloc(num_slots, sizeof(set_t*));  // allocate memory for the table
  ht->slots = num_slots;
  if (ht->table == NULL) {   // if error allocating memory
    mem_free(ht);
    return NULL;
  }
  // Initializing all elements of the table to be pointer to set
  for (int i = 0; i < num_slots; i++) {
    ht->table[i] = set_new();
  }
  return ht;
}

/**************** hashtable_insert ****************/
/* see hashtable.h for description */
bool
hashtable_insert(hashtable_t* ht, const char* key, void* item)
{
  if (ht == NULL || key == NULL || item == NULL) {
    return false;
  }
  set_t* set = getset(ht, key);
  bool op = set_insert(set, key, item);
  #ifdef MEMTEST
    mem_report(stdout, "End of hashtable_insert");
  #endif
  return op;
}

/**************** hashtable_find ****************/
/* see hashtable.h for description */
void* 
hashtable_find(hashtable_t* ht, const char* key)
{
  if (ht == NULL || key == NULL) {
    return NULL;
  }
  set_t* set = getset(ht, key);
  return set_find(set, key);
} 

/**************** hashtable_print ****************/
/* see hashtable.h for description */
void 
hashtable_print(hashtable_t* ht, FILE* fp, 
                     void (*itemprint)(FILE* fp, const char* key, void* item))
{
  if (fp == NULL) {
    return;
  }
  if (ht == NULL) {
    fprintf(fp, "(null)");
    return;
  }
  for (int i = 0; i < ht->slots; i++) {
    fprintf(fp, "Slot %d: ", i);
    set_t* set = ht->table[i];
    if (itemprint != NULL) {
      set_print(set, fp, itemprint);
    }
    fprintf(fp, "\n");
  }
}

/**************** hashtable_iterate ****************/
/* see hashtable.h for description */
void hashtable_iterate(hashtable_t* ht, void* arg,
                       void (*itemfunc)(void* arg, const char* key, void* item) )
{
  if (ht == NULL || itemfunc == NULL) {
    return;
  }
  for (int i = 0; i < ht->slots; i++) {
    set_t* set = ht->table[i];
    set_iterate(set, arg, itemfunc);
  }
}

/**************** hashtable_delete ****************/
/* see hashtable.h for description */
void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) )
{
  if (ht == NULL) {
    return;
  }
  for (int i = 0; i < ht->slots; i++) {
    set_t* set = ht->table[i];
    set_delete(set, itemdelete);
  }
  mem_free(ht->table);
  mem_free(ht);
  #ifdef MEMTEST
    mem_report(stdout, "End of hashtable_delete");
  #endif
}

