/***
 * pagedir.c - handles the pagesaver 
 * see pagedir.h for more details
 * 
 * Trung Nguyen, CS50. Lab 4 - Spring 2023
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"
#include "mem.h"
#include "file.h"
#include <string.h>

/**************** pagedir_init ****************/
bool pagedir_init(const char* pageDirectory)
{
  const char* fileName = "/.crawler";
  int length = strlen(fileName) + strlen(pageDirectory);
  char* pathName = mem_malloc(length + 1);
  strcpy(pathName, pageDirectory);
  strcat(pathName, fileName);

  FILE* fp;

  if ((fp = fopen(pathName, "w")) == NULL) {
    mem_free(pathName);
    return false;
  } else {
    fclose(fp);
    mem_free(pathName);
    return true;
  }
}

/**************** pagedir_save ****************/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
  const char* slash = "/";
  int docIDLen = snprintf(NULL, 0, "%d", docID);
  char* docIDStr = mem_malloc(docIDLen + 1);
  snprintf(docIDStr, docIDLen + 1, "%d", docID);

  int pathNameLen = strlen(pageDirectory) + strlen(docIDStr) + 1;   // 1 for slash
  char* pathName = mem_malloc(pathNameLen + 1);
  strcpy(pathName, pageDirectory);
  strcat(pathName, slash);
  strcat(pathName, docIDStr);

  FILE* fp;

  if ((fp = fopen(pathName, "w")) == NULL) {
    fprintf(stdout, "Error: cannot write to file %s \n", pathName);
  } else {
    fprintf(fp, "%s\n", webpage_getURL(page));
    fprintf(fp, "%d\n", webpage_getDepth(page));
    fprintf(fp, "%s\n", webpage_getHTML(page));
    fclose(fp);
  }
  
  mem_free(docIDStr);
  mem_free(pathName);
} 

/**************** pagedir_check ****************/
bool pagedir_check(const char* pageDirectory)
{
  if (pageDirectory == NULL) {
    return false;
  }
  const char* crawler = "/.crawler";
  const char* one = "/1";
  int pathNameLen = strlen(pageDirectory) + strlen(crawler);
  char* pathName = mem_malloc(pathNameLen + 1);
  strcpy(pathName, pageDirectory);
  strcat(pathName, crawler);

  FILE* fp;
  if ((fp = fopen(pathName, "r")) == NULL) {
    fprintf(stderr, "Error: not a valid directory with .crawler file.\n");
    mem_free(pathName);
    return false;
  }
  mem_free(pathName);
  fclose(fp);

  pathName = mem_malloc(strlen(pageDirectory) + strlen(one) + 1);
  strcpy(pathName, pageDirectory);
  strcat(pathName, one);
  if ((fp = fopen(pathName, "r")) == NULL) {
    fprintf(stderr, "Error: directory did not contain a valid 1 file.\n");
    mem_free(pathName);
    return false;
  }
  mem_free(pathName);
  fclose(fp);
  return true;
}

/**************** pagedir_read ****************/
webpage_t* pagedir_read(const char* pageDirectory, const int docID)
{
  const char* slash = "/";
  int docIDLen = snprintf(NULL, 0, "%d", docID);
  char* docIDStr = mem_malloc(docIDLen + 1);
  snprintf(docIDStr, docIDLen + 1, "%d", docID);

  int pathNameLen = strlen(pageDirectory) + strlen(docIDStr) + 1;   // 1 for slash
  char* pathName = mem_malloc(pathNameLen + 1);
  strcpy(pathName, pageDirectory);
  strcat(pathName, slash);
  strcat(pathName, docIDStr);

  FILE* fp;
  char* url;
  char* depthStr;
  int depth;
  char* html;

  if ((fp = fopen(pathName, "r")) != NULL) {
    url = file_readLine(fp);
    depthStr = file_readLine(fp);
    depth = atoi(depthStr);
    html = file_readFile(fp);
    webpage_t* page = webpage_new(url, depth, html);
    mem_free(docIDStr);
    mem_free(pathName);
    mem_free(depthStr);
    fclose(fp);
    return page;
  }

  mem_free(docIDStr);
  mem_free(pathName);
  return NULL;
}