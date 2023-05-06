# CS50 TSE Querier

## Design Spec

Written by Trung Nguyen CS50, Spring 2023. Based on the Crawler's [Design Spec](https://github.com/CS50Spring2023/home/blob/main/labs/tse/crawler/DESIGN.md) of CS50.

According to the [Querier Requirements Spec](https://github.com/CS50Spring2023/home/blob/main/labs/tse/querier/REQUIREMENTS.md), the TSE Querier is standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin.

### User interface

The querier has two interfaces with the users: on the command line and when it asks the user for query. It must have two arguments. After the program has succesfully verified arguments, it will repeatedly asks user for input, until EOF (Ctrl+D on keyboard). The second interfaces (asks user for inputs - stdin) can be replaced with a file.

```bash
querier pageDirectory indexFilename
Query?: input
```

or

```bash
querier pageDirectory indexFilename < testfile
```

### Inputs and Outputs

**Input**: the querier constructs an index from `indexFilename`, takes in query from user input and retrives needed `url` from `pageDirectory`.

**Ouput**: Print out, in decreasing order of `score`,  all matching `url`, with its `docID` and `score` to stdout.
