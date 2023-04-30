/**
 * word.c - normalize a word.
 * see word.h for more details
 * 
 * Trung Nguyen, Dartmouth CS50. Spring 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**************** normalizeWord ****************/
void normalizeWord(char* word)
{
  for (int i = 0; word[i] != '\0'; i++) {
    word[i] = tolower(word[i]);
  }
}