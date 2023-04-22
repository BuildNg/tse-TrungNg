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
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
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
  snprintf(docIDstr, docIDLen + 1, "%d", docID);

  int pathNameLen = strlen(pageDirectory) + strlen(docIDstr) + 1;   // 1 for slash
  char* pathName = mem_malloc(pathNameLen + 1);
  strcpy(pathName, pageDirectory);
  strcat(pathName, slash);
  strcat(pathName, docIDStr);

  FILE* fp;

  if ((fp = fopen(pathName, "w")) == NULL) {
    fprintf(stdout, "Error: cannot write to file %s \n", pageDirStr);
  } else {
    fprintf(fp, "%s\n", webpage_getURL(page));
    fprintf(fp, "%s\n", webpage_getDepth(page));
    fprintf(fp, "%s\n", webpage_getHTML(page));
    fclose(fp);
  }
  
  mem_free(docIDStr);
  mem_free(pathName);
} 