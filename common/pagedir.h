/**
 * pagedir.h - header file for pagedir module
 * re-usable module to handles the pagesaver (writing a page to the pageDirectory)
 * and marking it as a Crawler-produced pageDirectory.
*/

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"

/**************** pagedir_init ****************/
/* construct the pathname for the .crawler file in that directory
 * open the file for writing; on error, return false.
 * close the file and return true.
 *
 * Caller provides:
 *   pageDirectory to be marked with .crawler file.
 * We return:
 *   on error, return false; true if successfully write
 */
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* construct the pathname for the page file in pageDirectory
 *
 * Caller provides:
 *   valid page, pageDirectory, and docID
 * We do:
 *  open that file for writing
 *  print the URL
 *  print the depth
 *  print the contents of the webpage
 *  close the file
 *  on error, print out a message to stderr
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_check ****************/
/**
 * check if a page directory contains readable .crawler and '1' files or not
 * 
 * Caller provides:
 *   valid pointer to a string (pageDirectory)
 * We do:
 *   verify whether the directory contains readable .crawler and '1',
 *   i.e the directory is created by running crawler program.
*/
bool pagedir_check(const char* pageDirectory);

/**************** pagedir_read ****************/
/**
 * read a page (with the docID) from the directory.
 * 
 * Caller provides:
 *   valid pointer to a string (pageDirectory)
 *   valid integer docID, must be positive
 * We do:
 *   return NULL if:
 *     there is no file readable with the name 'docID' in pageDirectory, or
 *     failed to create a webpage_t due to memory issues.
 *   otherwise, return the pointer to a the file read - in webpage_t module.
 * Caller is responsible for:
 *   calling webpage_delete on the returned pointer after done. 
*/
webpage_t* pagedir_read(const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H