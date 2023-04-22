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
#include <../libcs50/hashtable.h>
#include <../libcs50/webpage.h>
#include <../libcs50/bag.h>
#include <../libcs50/mem.h>
#include <../common/pagedir.h>

int main(const int argc, char* argv[])
{
  char* seedURL = NULL;
  char* pageDirectory = NULL;
  int maxDepth = 0;

  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);

}

static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
  // check the number of arguments
  if (argc != 4) {
    fprintf(stderr, "Incorrect number of arguments. Usage: crawler seedURL pageDirectory maxDepth\n");
    exit(99);
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
    exit(1);
  }
  strcpy(*seedURL, normalizedURL);
  free(normalizedURL);
  if (!isInternalURL(*seedURL)) {         // check if the url is internal
    fprintf(stderr, "crawler: '%s' is not an internal URL.\n", *seedURL);
    exit(2);
  }

  // parse the directory
  *pageDirectory = mem_malloc(strlen[argv[2]] + 1);
  if (*pageDirectory == NULL) {
    fprintf(stderr, "Failed to allocate memory for pageDirectory.\n");
    exit(100);
  }
  strcpy(*pageDirectory, argv[2]);
  if (!pagedir_init(*pageDirectory)) {     // try to initiate the directory
    fprintf(stderr, "crawler: cannot initiate pageDirectory '%s'.\n", *pageDirectory);
    exit(3);
  }

  // parse the maximum depth
  char extraChar;
  if (sscanf(argv[3], "%d%c", maxDepth, &extraChar) != 1) {
    fprintf(stderr, "crawler: invalid integer '%s' for maxDepth.\n", argv[3]);
    exit(4);
  }
  if (*maxDepth < 0 || *maxDepth > 10) {    // check if the integer is in specified range
    fprintf(stderr, "crawler: integer %d out of range [0,1,..,10].\n", *maxDepth);
    exit(5);
  }
}

static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  // initialize hashtable
  hashtable_t* ht = hashtable_new(200);
  if (ht == NULL) {
    fprintf(stderr, "Hashtable cannot be initialized (memory problem).\n");
    exit(100);
  }

  // initialize the bag and add a webpage representing the seedURL at depth 0
  bag_t* bag = bag_new();
  if (bag == NULL) {
    fprintf(stderr, "Bag cannot be initialized (memory problem).\n");
    exit(100);
  }
  webpage_t* currPage = webpage_new(seedURL, 0, NULL);
  bag_insert(currPage);
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
  bag_delete(ht, webpage_delete);
}

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
      fprintf(stderr, "Failed to normalize URL.\n");
      mem_free(normalizedURL);
      continue;
    }
    if (!isInternalURL()) {
      mem_free(normalizedURL);
      continue;
    }
    bool newURL = hashtable_insert(pagesSeen, normalizedURL, "");
    if (newURL) {
      webpage_t* newPage = webpage_new(normalizedURL, depth + 1, NULL);
      bag_insert(pagesToCrawl, newPage);
    }
    mem_free(normalizedURL);
  }
}
