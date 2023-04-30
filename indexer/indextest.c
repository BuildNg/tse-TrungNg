/**
 * indextest.c - a C program which load an index file
 * into an inverted-index data structure,
 * create a new file and and write that index to that file.
 * These two files should be the same - compare them by indexcmp.
 * 
 * Trung Nguyen, CS50. Spring 2023 - Lab 5.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

/**************** function prototypes ****************/
static index_t* loadOldIndex(const char* oldIndexFilename);
static void writeNewIndex(index_t* index, const char* newIndexFilename);

/**************** main ****************/
/**
 * Check whether there are exactly 2 arguments,
 * load index from file with loadOldIndex and
 * print out the index with writeNewIndex
 * 
 * Caller provides: argc, argv
*/
int main(const int argc, char* argv[]) 
{
  char* oldIndexFilename;
  char* newIndexFilename;

  if (argc != 3) {
    fprintf(stderr, "Incorrect number of arguments. Usage: ./indextest oldIndexFilename newIndexFilename");
    exit(99);
  }
  oldIndexFilename = argv[1];
  newIndexFilename = argv[2];
  index_t* index = loadOldIndex(oldIndexFilename);
  writeNewIndex(index, newIndexFilename);
  index_delete(index);
  return 0;
}

/**************** loadOldIndex ****************/
/**
 * Try reading from file and load data into an index.
 * 
 * Caller provides:
 *   valid pointer to a string (file name)
 * 
 * We do:
 *   try to read the file
 *   load the data into an index by calling index_load
 * We assume:
 *   the file is properly formatted according to the spec.
 * We return:
 *   the pointer to the index we just loaded.
*/
static index_t* loadOldIndex(const char* oldIndexFilename)
{
  FILE* fp;
  if ((fp = fopen(oldIndexFilename, "r")) != NULL) {
    index_t* index = index_load(fp);
    fclose(fp);
    return index;
  }
  fclose(fp);
  return NULL;
}

/**************** writenewIndex ****************/
/**
 * Print the index given into output file
 * Check whether the two files (oldIndex, newIndex) are the same by calling
 * ~/cs50-dev/shared/tse/indexcmp oldIndex newIndex
 * 
 * Caller provides:
 *   valid pointer to an index, created by loadOldIndex
 *   valid pointer to a string (output file)
 * We do:
 *   try opening that file to write
 *   print out the index, if not null.
*/
static void writeNewIndex(index_t* index, const char* newIndexFilename)
{
  if (index == NULL) {
    return;
  }
  FILE* fp;
  if ((fp = fopen(newIndexFilename, "w")) != NULL) {
    index_print(fp, index);
  }
  fclose(fp);
}
