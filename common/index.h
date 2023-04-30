/**
 * index.h: header file for index.c
 * a module providing the data structure to represent the in-memory index 
 * essentially a hashtable with (key, item) as (word, (docID, #occurences))
 * item (docID, #count) is represented by counters module.
 * also contain functions to read (index_load) and write (index_print) index files.
 * 
 * A lot of functions are wrappers from hashtable.h.
 * 
 * Trung Nguyen, CS50. Spring 2023.
*/

#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "counters.h"
#include "mem.h"
#include "file.h"

/**************** global types ****************/
typedef hashtable_t index_t;

/**************** functions ****************/

/**************** index_new ****************/
/**
 * Create an empty index, a hastable with words as keys, and counters as items.
 * 
 * Caller provides: 
 *    number of slots to be used for the hashtable (must be > 0).
 * We return:
 *   pointer to the new index; return NULL if error.
 * We guarantee:
 *   index is initialized empty.
 * Caller is responsible for:
 *   later calling index_delete.
*/
index_t* index_new(int num_slots);

/**************** index_insert ****************/
/**
 * Insert item, identified by word (a string), and the doc ID (key to the counter, which is the item).
 * In other words: word -> docID -> #occurences
 *
 * Caller provides:
 *   valid pointer to index, valid string for word, valid integer docID
 * We do:
 *   if the word already exists, then increment its count indicated by the docID
 *   if not, create a new counter as the item for the word
 *   and then increment the count of docID to 1.
 * Notes:
 *   The word string is copied for use by the index; that is, the module
 *   is responsible for allocating memory for a copy of the word string, and
 *   later deallocating that memory; thus, the caller is free to re-use or 
 *   deallocate its word string after this call.   
*/
void index_insert(index_t* index, const char* word, const int docID);

/**************** index_save ****************/
/**
 * Save a word (string), docID (integer), count (integer) occurences to the index. 
 * Used to construct the inverse index.
 * 
 * Caller provides:
 *   valid pointer to index, valid string for word, valid integers docID and count.
 * We do:
 *   if the word already exists, change the count indicated by its docID to 'count'
 *   if not, create a new counter as the item for the word, and set its count to 'count'.
 * Notes:
 *   The word string is copied for use by the index; that is, the module
 *   is responsible for allocating memory for a copy of the word string, and
 *   later deallocating that memory; thus, the caller is free to re-use or 
 *   deallocate its word string after this call.  
*/
void index_save(index_t* index, const char* word, const int docID, const int count);

/**************** index_load ****************/
/**
 * Read data (in noted format) from a file
 * construct and return an index based on the data.
 * 
 * Caller provides:
 *   valid pointer to a file, which contains correct format for an index
 * We do:
 *   create a new (empty) index; return NULL if error
 *   read the data in line by line, taking the first word as the key
 *   continue to read each pair of integers as (docID, count) on the same line,
 *   add that data to the newly created index by calling index_save.
 * We return:
 *   an index from the file.
*/
index_t* index_load(FILE* fp);

/**************** index_iterate ****************/
/**
 * Wrapper function from hashtable. 
 * Iterate over all items (counters) in the index, in undefined order.
 * 
 * Caller provides:
 *   valid pointer to index, 
 *   arbitrary void*arg pointer,
 *   itemfunc that can handle a single (key, item) pair.
 * We do:
 *   nothing, if ht==NULL or itemfunc==NULL.
 *   otherwise, call the itemfunc once for each item, with (arg, key, item).
 * Notes:
 *   the order in which index items are handled is undefined.
 *   the index and its contents are not changed by this function,
 *   but the itemfunc may change the contents of the item.
*/
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));

/**************** index_print ****************/
/**
 * Print the whole index, in noted format such that index_load can read them
 * 
 * Caller provides:
 *   a valid pointer to index, and a FILE for writing the output.
 * We do:
 *   nothing, if ht == NULL or fp == NULL.
 *   otherwise, print each line containing exactly one word
 *   and one or more pairs of (docID, count), in the following format:
 *   word docID count [docID count]...
*/
void index_print(FILE* fp, index_t* index);

/**************** index_find ****************/
/**
 * Wrapper function from hashtable.
 * Return the counter associated with the given word.
 *
 * Caller provides:
 *   valid pointer to index, valid string for word.
 * We return:
 *   pointer to the counter corresponding to the given word, if found;
 *   NULL if hashtable is NULL, word is NULL, or word is not found.
 * Notes:
 *   the index is unchanged by this operation.
*/
counters_t* index_find(index_t* index, const char* word);

/**************** index_delete ****************/
/**
 * Delete the entire index.
 * 
 * Caller provides:
 *   a valid pointer to an index
 * We do:
 *   nothing, if the index is NULL
 *   delete all the keys (which are 'word' strings) 
 *   and all counters with its (key, count) pairs,
 *   as well as freeing all possible memories.
 * Notes:
 *   the order in which items are deleted is undefined.
*/
void index_delete(index_t* index);

#endif // __INDEX_H