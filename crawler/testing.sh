#!/bin/bash
# Testing examples for crawler.c
# Trung Nguyen, Dartmouth CS50. Spring 2023 - Lab 3.

# Testing erronous arguments
# No arguments
./crawler 

# One argument
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html

# Two arguments 
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-1

# Three arguments, not internal webpage
./crawler http://captive.apple.com/ ../data/invalid 1

# Three arguments, depth out of bound
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-1 -1

# Three arguments, directory does not exist
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/xyz 5

# Four arguments
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-1 2 CS50

# Valgrind test, for memory-leak 
myvalgrind='valgrind --leak-check=full --show-leak-kinds=all'
mkdir ../data/valgrind_toscape
$myvalgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/valgrind_toscape 1

# Testing letters
# Depth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-0 0

# Depth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-1 1
 
# Depth 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-2 2

# Depth 10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-10 10

# Testing toscrape
# Depth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-0 0

# Depth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-1 1

# Depth 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-2 2

# Depth 3
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-3 3

# Testing wiki
# Depth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia ../data/wiki-0 0

# Depth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia ../data/wiki-1 1

# Depth 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia ../data/wiki-2 2






