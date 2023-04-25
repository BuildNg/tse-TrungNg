/**
 * crawler.c - a program that crawls the web and retrieves webpages starting from a "seed" URL. 
 * It parses the seed webpage, extracts any embedded URLs, 
 * then retrieves each of those pages, recursively, but limiting its exploration to a given "depth".
 * 
 * Trung Nguyen, Dartmouth CS50. Lab 4 - Spring 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "webpage.h"
#include "bag.h"
#include "mem.h"
#include "pagedir.h"

static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/**************** main ****************/
/* Call the parseArges function to parse command line arguments and crawl the web using crawl function
 *
 * Caller provides:
 *   arc, argv
 */

int main(const int argc, char* argv[])
{
  char* seedURL = NULL;
  char* pageDirectory = NULL;
  int maxDepth = 0;

  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);
  mem_free(pageDirectory);

  return 0;
}

/**************** parseArgs ****************/
/* Given arguments from the command line, extract them into the function parameters; 
 * return only if successful.
 *
 * Caller provides: argc, argv, seedURL (pointer to a pointer to URL), 
 * pageDirectory (pointer to a pointer to directory) ,maxDepth
 *   
 * We do:
 * for seedURL, normalize the URL if possible and validate it is an internal URL
 * for pageDirectory, call pagedir_init()
 * for maxDepth, ensure it is an integer in specified range
 * if any trouble is found, print an error to stderr and exit non-zero.
 */

static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
  // check the number of arguments
  if (argc != 4) {
    fprintf(stderr, "Incorrect number of arguments. Usage: crawler seedURL pageDirectory maxDepth\n");
    exit(99);
  }

  // parse the maximum depth
  char extraChar;
  if (sscanf(argv[3], "%d%c", maxDepth, &extraChar) != 1) {
    fprintf(stderr, "crawler: invalid integer '%s' for maxDepth.\n", argv[3]);
    exit(1);
  }
  if (*maxDepth < 0 || *maxDepth > 10) {    // check if the integer is in specified range
    fprintf(stderr, "crawler: integer %d out of range [0,1,..,10].\n", *maxDepth);
    exit(2);
  }

  // parse the URL
  *seedURL = mem_malloc(strlen(argv[1]) + 1);
  if (*seedURL == NULL) {
    fprintf(stderr, "Failed to allocate memory for seedURL.\n");
    exit(100);
  }
  char* normalizedURL = normalizeURL(argv[1]);          // try to normalize the url
  if (normalizedURL == NULL) {
    fprintf(stderr, "URL cannot be normalized.\n");
    exit(3);
  }
  strcpy(*seedURL, normalizedURL);
  free(normalizedURL);
  if (!isInternalURL(*seedURL)) {         // check if the url is internal
    fprintf(stderr, "crawler: '%s' is not an internal URL.\n", *seedURL);
    exit(4);
  }

  // parse the directory
  *pageDirectory = mem_malloc(strlen(argv[2]) + 1);
  if (*pageDirectory == NULL) {
    fprintf(stderr, "Failed to allocate memory for pageDirectory.\n");
    exit(100);
  }
  strcpy(*pageDirectory, argv[2]);
  if (!pagedir_init(*pageDirectory)) {     // try to initiate the directory
    fprintf(stderr, "crawler: cannot initiate pageDirectory '%s'.\n", *pageDirectory);
    exit(5);
  }
  return;
}

/**************** crawl ****************/
/* Given arguments from the command line, extract them into the function parameters; 
 * return only if successful.
 *
 * Caller provides: valid seedURl (pointer to URL), maxDepth, pageDirectory (pointer to directory)
 *   
 * We do:
 * crawling from seedURL to max depth and save pages in directory
 * call on pageScan to crawl out to possible pages
 */

static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  // initialize hashtable
  hashtable_t* ht = hashtable_new(200);
  if (ht == NULL) {
    fprintf(stderr, "Hashtable cannot be initialized (memory problem).\n");
    exit(100);
  }
  hashtable_insert(ht, seedURL, "");

  // initialize the bag and add a webpage representing the seedURL at depth 0
  bag_t* bag = bag_new();
  if (bag == NULL) {
    fprintf(stderr, "Bag cannot be initialized (memory problem).\n");
    exit(100);
  }
  webpage_t* currPage = webpage_new(seedURL, 0, NULL);
  bag_insert(bag, currPage);
  int docID = 1;

  // start crawling
  while ((currPage = bag_extract(bag)) != NULL) {
    bool html = webpage_fetch(currPage);
    if (html) {
      pagedir_save(currPage, pageDirectory, docID);
      if (maxDepth > webpage_getDepth(currPage)) {
        pageScan(currPage, bag, ht);
      }
      docID++;
    }
    webpage_delete(currPage);
  }

  // delete the hashtable and bag
  hashtable_delete(ht, NULL);
  bag_delete(bag, webpage_delete);
}

/**************** pageScan ****************/
/* extracts URLs from a page and processes each one
 *
 * Caller provides: valid page, pagesToCrawl (pointer a bag), pagesSeen (pointer to a hashtable)
 *   
 * We do:
 * Given a webpage, scan the given page to extract any links (URLs), ignoring non-internal URLs; 
 * for any URL not already seen before (i.e., not in the hashtable).
 * (check by try adding to hashtable, which will return false if URL already exists)
 * add the URL to both the hashtable pages_seen and to the bag pages_to_crawl
 * 
 */

static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
  int pos = 0;
  char* nextURL;
  char* normalizedURL;
  int depth = webpage_getDepth(page);

  while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
    normalizedURL = normalizeURL(nextURL);
    mem_free(nextURL);
    if (normalizedURL == NULL) {
      continue;
    }
    if (!isInternalURL(normalizedURL)) {
      mem_free(normalizedURL);
      continue;
    }
    bool newURL = hashtable_insert(pagesSeen, normalizedURL, "");
    if (newURL) {
      webpage_t* newPage = webpage_new(normalizedURL, depth + 1, NULL);
      bag_insert(pagesToCrawl, newPage);
    }
    else {
      mem_free(normalizedURL);
    }
  }
}
