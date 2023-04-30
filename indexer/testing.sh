#!/bin/bash
# Testing file for indexer.c and for the index module, using indextest.c
# Trung Nguyen, Dartmouth CS50. Spring 2023 - Lab 5.

# Testing erronous arguments

# 1. No arguments
./indexer

# 2. One argument 
./indexer ../data/letters-0

# 3. Three or more arguments
./indexer ../data/letters-0 ../data/letters0.index ../data/letters1.index

# 4. Invalid pageDirectory (non-existent path)
./indexer ../data/letters-19 ../data/letters19.index

# 5. Invalid pageDirectory (not a crawler directory)
./indexer ../data/common ../data/common.index

# 6. Invalid indexFile (non-existent path)
./indexer ../data/letters-0 ../abcxyz/letters0.index

# 7. Invalid indexFile (read-only directory)
./indexer ../data/letters-0 ~/cs50-dev/shared/letters0.index

# 8. Invalid indexFile (existing, but read-only file)
./indexer ../data/toscrape-1 ~/cs50-dev/shared/tse/indices/toscrape-1.ndx

# Test index and indexer on a variety of tests

# On letters-0
./indexer ../data/letters-0 ../data/letters0.index
./indextest ../data/letters0.index ../data/letters0.test
# Should print out nothing
~/cs50-dev/shared/tse/indexcmp ../data/letters0.index ../data/letters0.test 

# On toscrape-1
./indexer ../data/toscrape-1 ../data/toscrape1.index
./indextest ../data/toscrape1.index ../data/toscrape1.test
# Should print out nothing
~/cs50-dev/shared/tse/indexcmp ../data/toscrape1.index ../data/toscrape1.test 

# On wiki-2
./indexer ../data/wiki-2 ../data/wiki2.index
./indextest ../data/wiki2.index ../data/wiki2.test
# Should print out nothing
~/cs50-dev/shared/tse/indexcmp ../data/wiki2.index ../data/wiki2.test 

# Using valgrind for memory leak, on toscrape-1
myvalgrind='valgrind --leak-check=full --show-leak-kinds=all'
$myvalgrind ./indexer ../data/toscrape-1 ../data/toscrape1.index
$myvalgrind ./indextest ../data/toscrape1.index ../data/toscrape1.test
# Should print out nothing
~/cs50-dev/shared/tse/indexcmp ../data/toscrape1.index ../data/toscrape1.test 