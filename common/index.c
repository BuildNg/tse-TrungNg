/**
 * index.c - a module that map word to (docID, #occurences) pairs.
 * see index.h for more details
 * 
 * Trung Nguyen, CS50. Spring 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "file.h"
#include "hashtable.h"
#include "counters.h"

/**************** global types ****************/
typedef hashtable_t index_t;

/**************** local functions ****************/
static void index_print_helper(void* arg, const char* key, void* item);
static void item_print(void* arg, const int key, int count);
static void item_delete(void* item);

/**************** global functions ****************/
/* that is, visible outside this file */
/* see hashtable.h for comments about exported functions */

/**************** index_new ****************/
index_t* index_new(int num_slots)
{
  index_t* new_index = hashtable_new(num_slots);
  if (new_index == NULL) {
    fprintf(stderr, "Out of memory. \n");
    exit(100);
  }
  return new_index;
}
/**************** index_insert ****************/
void index_insert(index_t* index, const char* word, const int docID)
{
  counters_t* ctrs = hashtable_find(index, word);
  if (ctrs == NULL) {
    ctrs = counters_new();
    if (ctrs == NULL) {
      fprintf(stderr, "Out of memory. \n");
      exit(100);
    }
    hashtable_insert(index, word, ctrs);
  }
  counters_add(ctrs, docID);
}
/**************** index_save ****************/
void index_save(index_t* index, const char* word, const int docID, const int count)
{
  counters_t* ctrs = hashtable_find(index, word);
  if (ctrs == NULL) {
    ctrs = counters_new();
    if (ctrs == NULL) {
      fprintf(stderr, "Out of memory. \n");
      exit(100);
    }
    hashtable_insert(index, word, ctrs);
  }
  counters_set(ctrs, docID, count);
}

/**************** index_load ****************/
index_t* index_load(FILE* fp)
{
  int num_lines = file_numLines(fp);
  index_t* index = index_new(num_lines * 4 / 3);
  if (index == NULL) {
    fprintf(stderr, "Out of memory. \n");
    exit(100);
  }
  for (int i = 0; i < num_lines; i++) {
    char* word = file_readWord(fp);
    int docID, count;
    while (fscanf(fp, "%d %d ", &docID, &count) == 2) {
      index_save(index, word, docID, count);
    }
    mem_free(word);
  }
  return index;
}

/**************** index_iterate ****************/
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item)) 
{
  if (index != NULL && arg != NULL && itemfunc != NULL) {
    hashtable_iterate(index, arg, itemfunc);
  }
}

/**************** index_print ****************/
void index_print(FILE* fp, index_t* index) 
{
  if (fp == NULL || index == NULL) {
    fprintf(stderr, "Error: Index Can't be printed\n");
  } else {
      index_iterate(index, fp, index_print_helper);
    }
}

/**************** index_find ****************/
counters_t* index_find(index_t* index, const char* word) 
{
  counters_t* ctrs = hashtable_find(index, word);
  return ctrs;
}

/**************** index_delete ****************/
void index_delete(index_t* index) 
{
  if (index != NULL) {
    hashtable_delete(index, item_delete);
  }
}

/**************** local functions ****************/

/**************** index_print_helper ****************/
// A helper function to print the index
static void index_print_helper(void* arg, const char* key, void* item)
{
  FILE* fp = arg;
  if (fp == NULL || key == NULL || item == NULL) {
    fprintf(stderr, "Error: Index cannot be printed.\n");
  } else {
    counters_t* ctrs = item;
    fprintf(fp, "%s ", key);
    counters_iterate(ctrs, fp, item_print);
    fprintf(fp, "\n"); 
  }
} 

/**************** item_print ****************/
// A helper function to print the counter (which is the item inside index)
static void item_print(void* arg, const int key, int count)
{
  FILE* fp = arg;
  if (fp == NULL) { 
    fprintf(stderr, "Error: Index cannot be printed.\n");
  } else {
    fprintf(fp, "%d %d ", key, count);
  }
}

/**************** item_delete ****************/ 
// Delete the item of index (which is the counter)
static void item_delete(void* item)
{
  counters_delete((counters_t*) item);
}