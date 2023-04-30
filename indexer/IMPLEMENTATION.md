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

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

<!-- markdownlint-disable code-block-style -->
```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],  
                      char** pageDirectory, char** indexFilename);
static void indexBuild(const char* pageDirectory, const char* indexFilename);
static void indexPage(index_t* index, webpage_t* webpage, const int docID);
```
<!-- markdownlint-restore -->

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

When allocating memory, every pointers returned is checked whether it is null or not to check for out-of-memory errors. If this happens, print an error message to stderr, and exit `100`.

Errors of invalid `pageDirectory` are handled by `pagedir_check()`, a function within `pagedir` , while error of invalid `indexFilename` is handled by `parseArgs`.

## Libraries

The Indexer `(indexer.c)` depends on two libraries.

### libcs50.a

We leverage the modules of libcs50, most notably `bag`, `hashtable`, and `webpage`.
See that directory for module interfaces.

### common.a

The library contains 3 used modules: `pagedir`, `index`, and `word`.

## Testing

Here is the implementation-specific plan:

### Unit testing

A program `indextest.c` will serve as the unit test for the `index` module; it reads an index file into an internal `index` data structure, then write it out to a new file. It will check whether the data structure implemented, `index`, works properly.

### Integration testing

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments.
First, a sequence of invocations with erronous arguments, testing each of the possible mistakes can be made. Second, a run with both `indexer` and `indextest` for `letters-0`, `toscrape-1`, and `wiki-2`. `indextest` serves as a mean to validate the resulting index. We also invokes `indexcmp` to compare the old index (created by `indexer`), and the new index (created by `indextest`):

    ~/cs50-dev/shared/tse/indexcmp oldIndexPath newIndexPath

Finally, a run with valgrind with both `indexer` and `indextest` for `toscrape-1` to check for any memory leaks. We run this script with `bash -v testing.sh` so the output of the indexer is intermixed with the commands used to invoke the crawler. Verify correct behavior by studying the output, and by sampling the files created in the respective `indexFilename`.
