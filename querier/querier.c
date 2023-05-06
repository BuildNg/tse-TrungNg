/**
 * querier.c - a standalone program that 
 * reads the index file produced by the TSE Indexer,
 * and pages files produced by the TSE Querier,
 * to answer search queries submitted via stdin.
 * 
 * Trung Nguyen, Dartmouth CS50. Spring 2023 - Lab 6.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include "file.h"
#include "mem.h"
#include "index.h"
#include "counters.h"
#include "pagedir.h"
#include "webpage.h"

/**************** function prototypes ****************/
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static int query(char* indexFilename, char* pageDirectory);
static int preprocessQuery(char* query);
static void tokenize(char* queryStr, char** words, int* length);
static counters_t* processLogic(char** words, int length, index_t* index, int* exitStatus);
static void printResult(counters_t* ctrs, char* pageDirectory);
static void processAnd(counters_t* ctrsA, counters_t* ctrsB);
static void processOr(counters_t* ctrsA, counters_t* ctrsB);
static void countersPreprocess(void* args, const int key, const int count);
static void countersAddAll(void* args, const int key, const int count);
static void countersUnion(void* args, const int key, const int count);
static int findMin(int a, int b);
static void countersSize(void* args, const int key, const int count);
static void countersMax(void* args, const int key, const int count);
int fileno(FILE* stream);
static void prompt(void);


/**************** main ****************/
/**
 * Call the parseArgs function to parse command line arguments and
 * start querying for input.
*/
int main(const int argc, char* argv[])
{
  char* pageDirectory;
  char* indexFilename;

  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  int exitStatus = query(indexFilename, pageDirectory);
  return exitStatus;
}

/**************** parseArgs ****************/
/* Given arguments from the command line, extract them into the function parameters; 
 * return only if successful.
 *
 * Parameter: argc, argv, pageDirectory (pointer to a pointer to directory), 
 * indexFilename (pointer to a pointer to directory for output)
 *   
 * We do:
 * for pageDirectory, check whether it is produced by the Crawler
 * for indexFilename, check whether we can read the index
 * if any trouble is found, print an error to stderr and exit non-zero.
 */
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
  if (argc != 3) {
    fprintf(stderr, "Incorrect number of arguments. Usage: ./querier pageDirectory indexFilename.\n");
    exit(99);
  }
  
  *pageDirectory = argv[1];
  *indexFilename = argv[2];
  if (!pagedir_check(*pageDirectory)) {
    fprintf(stderr, "This directory is not produced by crawler.\n");
    exit(1);
  }

  FILE* fp;
  if ((fp = fopen(*indexFilename, "r")) == NULL) {
    fprintf(stderr, "Cannot read index file at %s.\n", *indexFilename);
    exit(2);
  }
  fclose(fp);
}

/**************** query ****************/
/**
 * Do the main work of querier.c.
 * Given the file containing webpages and file containing the index,
 * query for input, and print out url with matching contents,
 * in decreasing score.
 * 
 * Parameter: valid indexFilename, pageDirectory (all pointers to char)
 * 
 * We do:
 *  read the index from indexFilename with index_load()
 *  read the input in, line by line
 *  preprocess the input to remove space, with preprocessQuery() and tokenize()
 *  process its logic, using processLogic()
 *  print out the url with matching contents, in decreasing score,
 *  using printResult()
 * 
 * if any trouble is found:
 *  memory error: exit
 *  query invalid: free memory and continue to next line
*/
static int query(char* indexFilename, char* pageDirectory)
{
  FILE* fp = fopen(indexFilename, "r");
  char* queryStr;
  char** words;
  int length = 0;
  counters_t* result;
  int exitStatus = 0;

  // load the index
  index_t* index = index_load(fp);
  fclose(fp);
  prompt();
  // read input in, line by line
  while ((queryStr = file_readLine(stdin)) != NULL) {
    // preprocess to get # words and normalize words
    length = preprocessQuery(queryStr);
    if (length <= 0) {          // if invalid input
      prompt();
      mem_free(queryStr);
      continue;
    }

    // allocate memories for array of pointers to each word
    words = mem_malloc(length * sizeof(char*));
    if (words == NULL) {
      fprintf(stderr, "Memory error.");
      exitStatus = 100;
      mem_free(queryStr);
      break;
    }

    // process to remove space, slice input into array of words
    tokenize(queryStr, words, &length);

    // Print the processed query
    for (int i = 0; i < length; i++) {
      fprintf(stdout, "%s", words[i]);
      if (i != length - 1){
       fprintf(stdout, " ");
      }
    }
    printf("\n");

    // get the counters, which have keys as docID, that satisfies the input.
    result = processLogic(words, length, index, &exitStatus);
    if (result == NULL) {
      mem_free(words);
      mem_free(queryStr);
      if (exitStatus == 100) {
        break;
      }
      prompt();
      continue;
    } 

    // print out the url, in decreasing order of scores
    printResult(result, pageDirectory);
    mem_free(words);
    mem_free(queryStr);
    counters_delete(result);
    prompt();
  }

  // clean up
  index_delete(index);
  return exitStatus;
}

/**************** preprocessQuery ****************/
/**
 * given the input, preprocess the query to normalize all words, 
 * notify error if input contains non-alphabetic letters, and get the # words.
 * 
 * Parameter:
 *  valid pointer to a string (input)
 * We do:
 *  go through all chars of the string, try to normalize words
 *  and count the number of words in the string.
 * We return:
 *  -1, if there are non-alphabetic character
 *  else, return the number of words in the input.
*/
static int preprocessQuery(char* queryStr) {
  int length = 0;
  char* s = queryStr;
  bool newWord = true;
  while (*s != '\0') {
    if (isspace(*s)) {
      s++;
      newWord = true;
      continue;
    } else if (!isalpha(*s)) {
      fprintf(stderr, "Bad character '%c' in query.\n", *s);
      return -1;
    } 

    *s = tolower(*s);
    if (!newWord) {
      s++;
      continue;
    }
    length++;
    newWord = false;
    s++;
  }
  return length;
}

/**************** tokenize ****************/
/**
 * given the input and the length, split the input 
 * into array of words, removing excess space.
 * 
 * Parameter:
 *  valid pointer to queryStr, valid pointer to array words, valid integer
 * 
 * We assume:
 *  the integer 'length' has been obtained from 
 *    running preprocessQuery on 'queryStr'
 *  queryStr has been preprocessed (by preprocessQuery) and contains only valid characters
 * We do:
 *  each element of array words now point to the start of each word,
 *  separated by space in queryStr
 *  write '\0' at the memory right after the end of each word
*/
static void tokenize(char* queryStr, char** words, int* length)
{
  char* s = queryStr;
  bool newWord = true;
  bool addNull = false;
  int pos = 0;
  while (*s != '\0') {
    if (isspace(*s)) {
      if (addNull) {
        *s = '\0';
        addNull = false;
      }
      s++;
      newWord = true;
      continue;
    } else if (!newWord) {
      s++;
      continue;
    }
    words[pos] = s;
    newWord = false;
    addNull = true;
    s++;
    pos++;
  }
}

/**************** processLogic ****************/
/**
 * given the array of words and the index of all words, process the array and 
 * return the counters containing matching docID with its score.
 * 
 * Parameter:
 *  valid pointer to array of pointers to chars (words)
 *  valid length of the array
 *  valid pointer of an index, loaded from indexFilename
 *  pointer to an integer, the exitStatus
 * We do:
 *  first, verify if the input (words) is syntactically corect
 *  go through the array, taking the intersection or union
 *  of counters of each word as appropriate.
 * We return:
 *  NULL and change exitStatus to 100 if memory error
 *  NULL, if input is not syntactically correct
 *  else, a pointer to counters_t containing pairs of matching (docID, count)
*/
static counters_t* processLogic(char** words, int length, index_t* index, int* exitStatus)
{
  // Verify the start/end
  if (strcmp(words[0], "and") == 0) {
    fprintf(stderr, "Error: 'and' cannot be first\n");
    return NULL;
  } else if (strcmp(words[0], "or") == 0) {
    fprintf(stderr, "Error: 'or' cannot be first\n");
    return NULL;
  } else if (strcmp(words[length - 1], "and") == 0) {
    fprintf(stderr, "Error: 'and' cannot be last\n");
    return NULL;
  } else if (strcmp(words[length - 1], "or") == 0) {
    fprintf(stderr, "Error: 'or' cannot be last\n");
    return NULL;
  }

  // 2 status: 0 if the last word is a normal word, -1 if it is 'or' / 'and'
  int lastWord = 0;
  // Verify consecutive
  for (int i = 0; i < length; i++) {
    if (lastWord == -1) {
      if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
        fprintf(stderr, "'%s' and '%s' cannot be adjacent \n", words[i-1], words[i]);
        return NULL;
      }
    }
    if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
      lastWord = -1;
    } else {
      lastWord = 0;
    }
  }

  // result is the 'running sum', and temp is the 'running product'
  counters_t* result = counters_new();
  counters_t* temp = counters_new();
  if (result == NULL || temp == NULL) {
    fprintf(stderr, "Out of memory.\n");
    counters_delete(result);
    counters_delete(temp);
    *exitStatus = 100;
    return NULL;
  }
  // flag to indicate temp is just created anew by counters_new()
  bool flag = true;
  for (int i = 0; i < length; i++) {
    if (flag) {
      // note that calling processOr on an empty counter with another counter
      // effectively copied the second onto the first
      processOr(temp, index_find(index, words[i]));
      flag = false;
    }
    // if the current word is not "or", continue intersect
    if (strcmp(words[i], "or") != 0) {
      processAnd(temp, index_find(index, words[i]));
    } else {
      processOr(result, temp);        // if the current word is "or", merge temp into result
      counters_delete(temp);
      temp = counters_new();
      if (temp == NULL) {
        fprintf(stderr, "Out of memory.\n");
        counters_delete(result);
        *exitStatus = 100;
        return NULL;
      }
      // turn on the flag
      flag = true;
    }
    if (i == length - 1) {
      // if at the last word, merge temp into result anyway
      processOr(result, temp);
      counters_delete(temp);
    }
  }
  return result;
}

/**************** printResult ****************/
/**
 * Given a counter and a pageDirectory (created from crawler),
 * print out the result in decreasing score.
 * 
 * Parameter: pointer to counter, pointer to string (pageDirectory)
 * 
 * We do:
 *  first, iterate over the counter to find # pairs with count > 0
 *  repeatedly find the current highest count, print it with url
 *  obtained from pageDirectory, before setting its count to 0.
*/
static void printResult(counters_t* ctrs, char* pageDirectory) {
  int maxCount[2];
  maxCount[0] = 0;        // hold the key associated with current highest count
  maxCount[1] = 0;        // current highest count
  int size = 0;           // # pairs with count > 0
  char* url;

  // find the sizem with helper function
  counters_iterate(ctrs, &size, countersSize);
  if (size == 0) {
    printf("No documents match. \n");
    return;
  }
  printf("Matches %i documents (ranked): \n", size);

  // repeatedly find the highest count, with helper function
  while (size > 0) {
    counters_iterate(ctrs, maxCount, countersMax);
    // load in the webpage
    webpage_t* currPage = pagedir_read(pageDirectory, maxCount[0]);
    url = webpage_getURL(currPage);
    printf("score %3i doc %3i: %s\n", maxCount[1], maxCount[0], url);
    webpage_delete(currPage);

    // set the highest count to 0 and resetting maxCount[1] to 0.
    counters_set(ctrs, maxCount[0], 0);
    maxCount[1] = 0;
    size--;
  }
}

/**************** processAnd ****************/
/**
 * Given two counters, turn ctrsA into the intersection of both sets.
 * Intersection: ctrsA now have all keys of both counters, but with the lower score of the two. 
 * score = 0 if a key only exists in one counter.
 * 
 * Parameters: valid pointers to ctrsA and ctrsB (two counters)
 * We do: 
 *  calling counters_iterate on them with 2 helper functions,
 *  countersPreprocess and countersAddAll, as detailed below.
 * Effects:
 *  ctrsA will now be the intersection of 2 counters
 *  ctrsB now has additional keys from ctrsA that does not exist before,
 *  but with count = 0, effectively does not change ctrsB much.
*/
static void processAnd(counters_t* ctrsA, counters_t* ctrsB)
{
  counters_iterate(ctrsA, ctrsB, countersPreprocess);
  counters_iterate(ctrsB, ctrsA, countersAddAll);
}

/**************** processOr ****************/
/**
 * Given two counters, turn ctrsA into the union of two sets.
 * Union: ctrsA now have all keys of both counters, but with higher score of the two.
 * 
 * Parameters: valid pointers to ctrsA and ctrsB (two counters)
 * We do:
 *  calling counters_iterate on ctrsB with helper countersUnion.
 * After the call: ctrsB unchanged, ctrsA is now the union.
*/
static void processOr(counters_t* ctrsA, counters_t* ctrsB)
{
  counters_iterate(ctrsB, ctrsA, countersUnion);

}

/**************** countersPreprocess ****************/
/**
 * Helper function for processAnd. If key does not exist, add it with count = 0.
*/
static void countersPreprocess(void* args, const int key, const int count)
{
  counters_t* ctrsB = args;
  if (counters_get(ctrsB, key) == 0) {
    counters_set(ctrsB, key, 0);
  }
}
/**************** countersAddAll ****************/
/**
 * Helper function for processAnd. 
 * Find the minimum between count (of ctrsB) and ctrsA's count of the current key,
 * and set it to ctrsA.
 * As we have preprocessed ctrsB, ctrsB holds all keys of ctrsA.
*/
static void countersAddAll(void* args, const int key, const int count)
{
  counters_t* ctrsA = args;
  int newCount = findMin(count, counters_get(ctrsA, key));
  counters_set(ctrsA, key, newCount);
}

/**************** countersIntersect ****************/
/**
 * Helper function for processOr. 
 * Compute the total of count (of ctrsB) and ctrsA's count of the current key,
 * and set it to ctrsA.
*/
static void countersUnion(void* args, const int key, const int count)
{
  counters_t* ctrsA = args;
  int newCount = count + counters_get(ctrsA, key);
  counters_set(ctrsA, key, newCount);
}


/**************** findMin ****************/
/**
 * find the min of a and b
 * 
 * Parameter: int a, b
 * We return the smaller of the two
*/
static int findMin(int a, int b) 
{
  return (a > b) ? b : a;
}

/**************** countersSize ****************/
/**
 * helper function used in printResult to find the size of counter
*/
static void countersSize(void* args, const int key, const int count)
{
  int* size = args;
  if (count > 0) {
    (*size)++;
  }
}

/**************** countersMax ****************/
/**
 * helper function used in printResult to find the current max of count
*/
static void countersMax(void* args, const int key, const int count)
{
  int* maxCount = args;
  // update if find a higher count
  if (count > maxCount[1]) {
    maxCount[0] = key;
    maxCount[1] = count;
  }
}

/**************** prompt ****************/
/**
 * for turning off the prompt if not read in from stdin
*/
static void prompt(void)
{
  if (isatty(fileno(stdin))) {
    printf("Query?: ");
  }
}
