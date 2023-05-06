# CS50 TSE Querier

## Implementation Spec

Written by Trung Nguyen, CS50.

In this document we reference the [Querier Requirements Spec](https://github.com/CS50Spring2023/home/blob/main/labs/tse/querier/REQUIREMENTS.md) and the [Querier Design Spec](DESIGN.md) and focus on the implementation-specific decisions. Core contents:

- Data structures
- Control flow: Psuedocode for overall flow, and for each of the functions
- Detailed function prototypes and their parameters
- Error handling and recovery
- Testing plan

## Data structures

We used two main data structures: a `index` to hold mapping from words to pairs of `(docID, #count)`, and `counters`, which is keyed by `docID` and the items are `count`. Counters will be used extensively to calculate the score when we take the conjunction or disjunction of two counters, as in the Requirements Spec.

## Control flow

The Querier is implemented in `querier.c`, with seven main functions, and ten helper functions.

### main

The `main` function simply calls `parseArgs` and `query`, before exiting with status returned by `query`.

### parseArgs

Given the arguments from the command line, extract them into the function parameters; return only if successful.

- for `pageDirectory`, call `pagedir_check()` (a function in `common.a` library) to ensure that this directory exists, and that it is created by the Crawler and contains a readable `1` file.
- for `indexFilename`, check whether this file exists and readable.
- if any trouble is found, print an error to stderr and exit non-zero.

### query

The central function of `querier.c`, which will call four other important functions: `preprocessQuery`, `tokenize`, `processLogic`, `printResult`. Pseudocode:

    open input, load index in from indexFilename
    close input
    start prompting user for input by calling prompt
    read input line by line, until EOF is reached;
        call preprocessQuery to validate and preprocess the input, and get the number of words (length) as return
        if length is not positive:
            free the line, and prompt again
        allocate memories for array of pointers to words
        if failed: 
            free the line 
            return non-zero status back to main
        call tokenize with proprocessed input, array and length 
        print out the tokenized input for users to see
        call processLogic with array, its length, exitStatus, index, recieve back a pointer to counter
        if counter is NULL:
            free the words and the array
            if exitStatus is non-zero, return that status back to main.
            if not, prompt again
        call printResult with pageDirectory and the counter
        free the words and the array
        delete the counter
        prompt again
    delete the index

### preprocessQuery

A function to preprocess the input query. It checks if each character is either space or an alphabetic letter, which it will immediately return `-1` back to `query`, and normalizes all alphabetic characters. It will return the number of words in the query. Pseudocode:

    initialize length = 0, boolean newWord = true
    iterating through each letter of input, until EOF:
        if it is a space:
            set newWord to true
            continue to next letter
        else if it is not alphabetic:
            print an error message to stdout
            return -1
        normalize the letter
        if newWord is false
            continue to next letter
        increase length by 1
        set newWord to false
        continue to next letter
    return length

### tokenize

A function to splice preprocessed input into an array of pointers to words. As the input has been processed by `preprocessQuery`, no errors are expected. Pseudocode:

    initialize boolean newWord = true, addNull = false
    initialize pos (current position of array to write) to 0
    while the current letter is not EOF
        if it is a space
            if addNull is true
                overwrite the letter to '\0'
                set addNull to false
            set newWord to true
            continue to the next letter
        else if newWord is false
            continue to the next letter
        set arr[pos] to address of current letter
        set newWord to false
        set addNull to true
        increase pos by 1
        move on to the next letter

### processLogic

A function that takes in array of words, its length, an index, and exitStatus and return a counter containing matching docIDs and counts. This function checks for any syntactic errors; if there are none, start computing all matching results. If memory error occurs, change `exitStatus` to non-zero, and return NULL. More explanation can be found as comments in the file `querier.c`. This function also called two helpers, `processOr` and `processAnd` as noted in the Design Specs. Pseudocode:

    check if the first/end word of the array is 'or' or 'and'
        if yes, print error message to stderr and return NULL
    initialize integer lastWord = 0
    iterate through the whole array
        if lastWord is -1
            if the current word is 'or' or 'and'
                print an error, return NULL
        if the current word is 'or' or 'and'
            set lastWord to -1
        else, set lastWord to 0
    create a new counter_t*: result       // running sum
    create a new counter_t*: temp         // running product
    check if there is memory error
        if yes, print an error message to stderr
        free both counters
        change exitStatus to non-zero and return NULL
    initialize boolean flag = true    // to check if temp is just created anew
    // note that we know there is no syntactic error at this point
    iterate through the whole array
        if flag is true:
            call processOr on temp and the counter keyed by current word in the index 
            // from this point onwards call it the current counter
            turn off the flag (to false)
        if the current word is not "or":
            call processAnd on temp and the current counter
        else:
            call processOr on result and temp
            delete temp
            create a new counter_t* temp
            if there is memory error:
                delete result
                change exit status to non-zero and return NULL
            turn on the flag (to true)
        if the word is the last in the array:
            call processOr on result and temp
            delete temp
    return result

### printResult

A function that takes a counter, a `pageDirectory` (created by crawler) and print out the results for query, as noted in Designs Spec. It utilize two smaller helper functions, `countersSize` to help find the # of (docID, count) pairs of a counter and `countersMax` to help find the pair with highest count.

    initialize integer array of size 2
    set arr[0] = arr[1] = 0
    initialize integer size to 0

    call counters_iterate on counter, size and counterSize
    print out appropriate messages depending whether size is zero or not
    while size is still greater than zero:
        call counters_iterare on counter, array and countersMax
        load in the webpage with docID = arr[0] from pageDirectory
        get its url, and print out results in format specified in Designs Spec
        delete the webpage
        set the count of the highest pair to 0
        reset arr[1] to 0
        decrease size by 1

### Helper

Here are the helper functions to implement "other modules" in the Design Spec.

#### processAnd

This helper function find the intersection between two counters. It sets the first counter to be the intersection, and it makes no significant changes to the other. It has two additional helpers.

- Call `counters_iterate` on the first, second (counters) and helper `countersPreprocess`
- Call `counters_iterate` on the second, first (counters) and helper `countersAddAll`

in which:

- `countersPreprocess` will simply check if the current key (in the first) is present in the second or not. If not, insert it with count `0`. Now the second counters have all the keys in the first, but as all new keys have count `0`, it does not change significantly.
- `countersAddAll` will simply get the minimum between the current count (of the second) and the count of the first. It will then set that minimum for the current key in the first counter. Note that if the first does not have the key, the minimum will be `0`, which is correct. It uses a small helper `findMin`, which simply find the minimum between two numbers.

#### processOr

A simpler helper function to find the union between two counters. It sets the first counter to be the union, and it makes no change to the second counter. Note that if this function is called on a new counter and a second counter, it effectively copies all the data from the second onto the first (this is used in `processLogic`). It only has one additional helper.

- Call `counters_iterate` on the second, first (counters) and helper `countersUnion`.

in which

- `countersUnion` simply compute the sum of the current count (of the second counter) and the count of the current key in the first counter. It will then set that sum for the current key in the first counter.

### Functions (given by the hints)

Two additional helper functions are given from the [Querier Page](https://github.com/CS50Spring2023/home/tree/main/labs/tse/querier).

#### fileno

A helper given by C to check the file descriptor associate with a FILE* object.

#### prompt

A helper which print out "Query?" to keep asking user for input, if `stdin`, according to `fileno`, is indeed from the terminal.

### Other functions

The program `querier.c` also used many modules written for `indexer.c` and `crawler.c`, which includes: `pagedir_check`, `pagedir_read`, `index_load` and `index_delete`. These functions are located at `../common` inside their respective modules. More details can be found on [Indexer Implementation Specs](https://github.com/CS50Spring2023/tse-BuildNg/blob/submit5/indexer/IMPLEMENTATION.md). The querier also utilizes many functions from `libcs50`, at `../libcs50`.

## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here.

<!-- markdownlint-disable code-block-style -->
```c
static void parseArgs(const int argc, char* argv[], 
                      char** pageDirectory, char** indexFilename);
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
```
<!-- markdownlint-restore -->

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

When allocating memory, every pointers returned is checked whether it is null or not to check for out-of-memory errors. If this happens, print an error message to stderr, and exit `100`.

Errors of invalid command line arguments are handled by `parseArgs`, which immediately exits. Errors of invalid syntax are handled by `processLogic`, which only sends a signal (NULL) to `query` to skip this query and continue to the next one.

## Testing plan

Here is an implementation-specific testing plan.

### Fuzz testing

The querier.c is tested using the `fuzzquery.c`, which is a form of black-box testing in which you fire thousands of random inputs at the program to see how it reacts.

### Integration testing

We write a script `testing.sh` that invokes the querier several times, with a variety of command-line arguments. We also create a collection of small test files (especially edge cases) to pipe into querier. Most of the tests (from second set of tests onwards) are tested based on index loaded from `toscrape2.index`.

First, a sequence of invocations with erronous arguments, testing each of the possible mistakes can be made.

Second, a sequence of invocations with test files piped into the querier, all with some syntax errors. This type of testing will help checkers easily verify our tests again.

Third, a test with valgrind. We run valgrind on both our own created tests (with some syntax error) and on random inputs created by `fuzzquery`.

Lastly, large-scale tests with thousand of inputs generated randomly by `fuzzquery`. The outputs are piped to data to prevent our output file `testing.out` to be too large; qe also mix in valgrind here to check. We also changed our index to a much larger one, `wiki2.index` for the final test with 5000 random inputs.
