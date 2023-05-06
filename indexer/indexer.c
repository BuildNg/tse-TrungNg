/**
 * indexer.c - a standalone program that reads the document files produced by the TSE crawler.
 * It then builds an index, and writes that index to a file. 
 * 
 * Trung Nguyen, Dartmouth CS50. Lab 5 - Spring 2023.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "file.h"
#include "mem.h"
#include "index.h"
#include "pagedir.h"
#include "word.h"
#include "webpage.h"
#include "hashtable.h"
#include "counters.h"

/**************** function prototypes ****************/
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void indexBuild(const char* pageDirectory, const char* indexFilename);
static void indexPage(index_t* index, webpage_t* webpage, const int docID);

/**************** main ****************/
/**
 * Call the parseArgs function to parse command line arguments and
 * start building the index.
*/
int main(const int argc, char* argv[])
{
  char* pageDirectory;
  char* indexFilename;

  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  indexBuild(pageDirectory, indexFilename);
  return 0;
}

/**************** parseArgs ****************/
/* Given arguments from the command line, extract them into the function parameters; 
 * return only if successful.
 *
 * Caller provides: argc, argv, pageDirectory (pointer to a pointer to directory), 
 * indexFilename (pointer to a pointer to directory for output)
 *   
 * We do:
 * for pageDirectory, check whether it is produced by the Crawler
 * for indexFilename, check whether we can create the file
 * if any trouble is found, print an error to stderr and exit non-zero.
 */

static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
  if (argc != 3) {
    fprintf(stderr, "Incorrect number of arguments. Usage: ./indexer pageDirectory indexFilename.\n");
    exit(99);
  }
  FILE* fp;
  *pageDirectory = argv[1];
  *indexFilename = argv[2];
  if ((fp = fopen(*pageDirectory, "r")) == NULL) {
    fprintf(stderr, "This directory does not exist.\n");
    exit(1);
  } 
  fclose(fp);
  if (!pagedir_check(*pageDirectory)) {
    fprintf(stderr, "This directory is not produced by crawler.\n");
    exit(1);
  }

  if ((fp = fopen(*indexFilename, "w")) == NULL) {
    fprintf(stderr, "Cannot write output to %s.\n", *indexFilename);
    exit(2);
  }
  fclose(fp);
}

/**************** indexBuild ****************/
/**
 * Create an index and start building the index from all files in pageDirectory,
 * print them to indexFilename.
 * 
 * Caller provides: pageDirectory, indexFilename
 * 
 * We do: 
 *  create a new index
 *  read all files, starting with 1 by calling indexPage to build the index
 *  keep increasing docID by 1 until the file does not exist anymore
 *  print index to indexFilename
*/
static void indexBuild(const char* pageDirectory, const char* indexFilename)
{
  index_t* index = index_new(750);
  int docID = 1;
  webpage_t* currPage;
  FILE* fp;

  while (true) {
    currPage = pagedir_read(pageDirectory, docID);
    if (currPage == NULL) {
      break;
    }
    indexPage(index, currPage, docID);
    webpage_delete(currPage);
    docID++;
  }

  fp = fopen(indexFilename, "w");
  index_print(fp, index);
  index_delete(index);
  fclose(fp);
}

/**************** indexPage ****************/
/**
 * Given an index, a webpage, and the current docID, build the index.
 * 
 * Caller provides: a valid pointer to an index, a webpage, and the current docID
 * 
 * We do:
 *   Parse through the whole webpage,
 *   add all words (after normalized) with length >=3 to the index.
*/
static void indexPage(index_t* index, webpage_t* webpage, const int docID)
{
  char* currWord;
  int pos = 0;

  // step through all words
  while ((currWord = webpage_getNextWord(webpage, &pos)) != NULL) {
    if (strlen(currWord) < 3) {
      mem_free(currWord);
      continue;
    }
    normalizeWord(currWord);
    index_insert(index, currWord, docID);
    mem_free(currWord);
  }
}


