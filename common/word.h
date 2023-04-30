/**
 * word.h - header file for word module
 * re-usable module, used by indexer/querrier
 * handles normalizing a word.
*/

#include <stdio.h>
#include <stdlib.h>

/**************** normalizeWord ****************/
/**
 * normalize a word by converting all character to lowercase.
 * 
 * Caller provides: 
 *    valid pointer to a valid word, which ends properly with '\0'.
 * 
 * We do:
 *    convert all character to lowercase.
*/
void normalizeWord(char* word);