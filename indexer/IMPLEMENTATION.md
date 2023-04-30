# CS50 TSE Indexer

## Implementation Spec

Written by Trung Nguyen CS50, Spring 2023. Based heavily of the Crawler's [Implementation Spec](https://github.com/CS50Spring2023/home/blob/main/labs/tse/crawler/IMPLEMENTATION.md) of CS50.

In this document, we focus on the implementation-specific decisions, by referencing the [Requirements Specification](https://github.com/CS50Spring2023/home/blob/main/labs/tse/indexer/REQUIREMENTS.md) and the [Design Specification](https://github.com/CS50Spring2023/home/blob/main/labs/tse/indexer/DESIGN.md). We focus on the core subset of implementation:

- Data structures
- Control flow: pseudo code for overall flow, and for each of the functions
- Detailed function prototypes and their parameters
- Error handling and recovery
- Dependencies
- Testing plan

## Data structures

We use one main data structure: 'index', which is a 'hashtable' maps each word found to a 'counter'. This counter maps 'docID' (file name which the word is found) to 'count' (how many times the word appears).
The index starts empty. As it is essentially a hashtable, we do not know exactly how many slots it needs, so we use 750.

## Control flow

The Indexer is implemented in one file, `indexer.c`, with four functions.

### main

The `main` function simply calls `parseArgs` and `indexBuild`, then exits zero.

### parseArgs

Given the arguments from the command line, extract them into the function parameters; return only if successful.

- for `pageDirectory`, call `pagedir_check()` (a function in `common.a` library) to ensure that this directory exists, and that it is created by the Crawler and contains a readable `1` file.
- for `indexFilename`, check whether this directory exists and writable.
- if any trouble is found, print an error to stderr and exit non-zero.

### indexBuild

This function reads files from `pageDirectory`, create an indexer and saving it in `indexFilename`. Pseudocode:

    initialize the index and set the id of page to 1
    while true
        take the webpage from pageDirectory with the current id
        break the loop if the webpage is not successfully created
        indexPage that webpage
        delete that webpage
        increase id by 1
    save the index to indexFilename
    delete the index

### indexPage

Given the current `webpage` and its `docID`, scan it for any words longer than 3 letters and add it to the index. Pseudocode:

    while the webpage still has another word:
        if the length of the word is smaller than 3: 
            free that word
            continue to next loop
        normalize the word
        insert the word into the index
        free that word

## Other modules

We leverage the modules of libcs50, located in `../libcs50`, most notably, `webpage`, `hashtable` and `counter`. See that directory for module interfaces. Particularly, we import the file to `webpage_t` data structure to use its function `webpage_getNextWord()` to keep getiing the next word in the file.

We also added the following modules/functions in `../common`:

### pagedir

We add the two more functions to `pagedir.c`, to handle the process of loading webpages from files in `pageDirectory`, as mentioned in the Design Spec.

Pseudocode for `pagedir_check`:

    construct the pathname for the ./crawler file in that directory
    open the pathname for reading; on error, return false.
    close the file
    construct the pathname for the ./1 file in that directory
    open the pathname for reading; on error, return false.
    close the file
    return true

Pseudocode for `pagedir_read`:

    construct the pathname, given the directory and the docID.
    open the file for reading, if success:
        read its url, depth and html
        initialize a new webpage_t from that three parameters
        free the url, depthString, and html
        close the file
        return the webpage
    return NULL otherwise

### index

This is the major data structure used by the Indexer, as mentioned in the Requirement and Design Spec. For more information, check `Data Structures` section above.
`index_t` is effectively a hashtable data structure.

Pseudocode for `index_new`:

    Call hashtable_new with number of slots given, return NULL on error.

Pseudocode for `index_insert`:

    Call hashtable_find on the word given to get a counter_t pointer
    if the pointer is null:
        initialize a new counter
        insert that counter as the item of the word for the index using hashtable_insert
    increment the count for the word by counters_add

Pseudocode for `index_save`:

    Call hashtable_find on the word given to get a counter_t pointer
    if the pointer is null:
        initialize a new counter
        insert that counter as the item of the word for the index using hashtable_insert
    change the count for the word by counters_set

Pseudocode for `index_load`:

    get the number of lines from input file
    initialize a new index with slots equal to 4/3 times the number of lines
    for all lines in the input file:
        read the first word as the key
        while we can still read a pair of two numbers into docID and count:
            call index_save on the key, docID and count
        free the word
    return the index

Pseudocode for `index_iterate`:

    call hashtable_iterate on all parameters given

Pseudocode for `index_find`:

    call hashtable_find on the word given
    return the pointer to the counter

Pseudocode for `index_print`:

    if file output or index is null, print an error message to stderr
    else, call index_iterate on index, fp and a helper function index_print_helper

Pseudocode for `index_print_helper`:

    let fp be the args
    check if any parameter is null, print an error message if yes
    if not: 
        print out the key
        let each item be a counter
        call counters_iterate with helper item_print
        print a new line

Pseudocode for `item_print`:

    let fp be the args, print an error message if fp is null
    else, print the pair (docID, count)

Pseudocode for `index_delete`:

    if the index is not null:
        call hashtable_delete on the index with helper function item_delete

Pseudocode for `item_delete`:

    call counters_delete

### word

This is a module with only one function: normalizing a word.

Pseudocode for `normalizeWord`:

    for every characters of the word:
        normalize it to lower case

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and are not repeated here.

<!-- markdownlint-disable code-block-style -->
```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],  
                      char** pageDirectory, char** indexFilename);
static void indexBuild(const char* pageDirectory, const char* indexFilename);
static void indexPage(index_t* index, webpage_t* webpage, const int docID);
```
<!-- markdownlint-restore -->

### pagedir.c

Detailed descriptions of each function's interface is provided in `pagedir.h`, and are not repeated here.

<!-- markdownlint-disable code-block-style -->
```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_check(const char* pageDirectory);
webpage_t* pagedir_read(const char* pageDirectory, const int docID);
```
<!-- markdownlint-restore -->

### index.c

Detailed descriptions of each function's interface is provided in `index.h` and `index.c`, and are not repeated here.

<!-- markdownlint-disable code-block-style -->
```c
index_t* index_new(int num_slots);
void index_insert(index_t* index, const char* word, const int docID);
void index_save(index_t* index, const char* word, const int docID, const int count);
index_t* index_load(FILE* fp);
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));
void index_print(FILE* fp, index_t* index);
counters_t* index_find(index_t* index, const char* word);
void index_delete(index_t* index);
static void index_print_helper(void* arg, const char* key, void* item);
static void item_print(void* arg, const int key, int count);
static void item_delete(void* item);
```
<!-- markdownlint-restore -->

### word.c

Detailed descriptions of each function's interface is provided in `word.h`, and are not repeated here.

<!-- markdownlint-disable code-block-style -->
```c
void normalizeWord(char* word);
```
<!-- markdownlint-restore -->

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

When allocating memory, every pointers returned is checked whether it is null or not to check for out-of-memory errors. If this happens, print an error message to stderr, and exit `100`.

Errors of invalid `pageDirectory` are handled by `pagedir_check()`, a function within `pagedir` , while error of invalid `indexFilename` is handled by `parseArgs`.

## Testing

Here is the implementation-specific plan:

### Unit testing

A program `indextest.c` will serve as the unit test for the `index` module; it reads an index file into an internal `index` data structure, then write it out to a new file. It will check whether the data structure implemented, `index`, works properly.

### Integration testing

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments.
First, a sequence of invocations with erronous arguments, testing each of the possible mistakes can be made. Second, a run with both `indexer` and `indextest` for `letters-0`, `toscrape-1`, and `wiki-2`. `indextest` serves as a mean to validate the resulting index. We also invokes `indexcmp` to compare the old index (created by `indexer`), and the new index (created by `indextest`):

    ~/cs50-dev/shared/tse/indexcmp oldIndexPath newIndexPath

Finally, a run with valgrind with both `indexer` and `indextest` for `toscrape-1` to check for any memory leaks. We run this script with `bash -v testing.sh` so the output of the indexer is intermixed with the commands used to invoke the crawler. Verify correct behavior by studying the output, and by sampling the files created in the respective `indexFilename`.
