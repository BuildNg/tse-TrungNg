#!/bin/bash
# Testing files for querier.c module.
# Trung Nguyen, Dartmouth CS50. Spring 2023 - Lab 6.

# Variables for main testing
pdir="../data/toscrape-2"
indx="../data/toscrape2.index"

# Section 1: Testing erronous arguments

# 1. No arguments
./querier

# 2. One argument
./querier ../data/letters-0

# 3. Three or more arguments
./querier ../data/letters-0 ../data/letters0.index ../data/letters0.test

# 4. Invalid pageDirectory (non-existent path)
./querier ../data/letters-100 ../data/letters0.index

# 5. Invalid pageDirectory (not a crawler directory)
./querier ../data/common ../data/letters0.index

# 6. Invalid indexFile (non-existent indexfile)
./querier ../data/letters-0 ../data/indexhaha

# Section 2: Testing with edge cases, invalid input, many spaces

# 1. Many extra spaces and tabs between words
./querier $pdir $indx < testfiles/spaces

# 2. Input contains non-alphabetic words
./querier $pdir $indx < testfiles/non-alphabetic

# 3. Input with word mistakenly seperared by spaces (between each letters)
./querier $pdir $indx < testfiles/mistakenspaces

# 4. Blank input
./querier $pdir $indx < testfiles/blank

# 5. Uppercase/lowercase mixing
./querier $pdir $indx < testfiles/fuzz-case

# 6. Test "and" / "or" appearing at the end or consecutively
## 10 lines of input, should all print error messages
./querier $pdir $indx < testfiles/wrongandor

# Section 3: Test with valgrind. Contains some invalid input.
myvalgrind="valgrind --leak-check=full --show-leak-kinds=all"
$myvalgrind ./querier $pdir $indx < testfiles/valgrindtest
$myvalgrind ./fuzzquerry $indx 10 139 | ./querier $pdir $indx > ../data/valgrind10_139

# Section 4: Test with fuzzquerry and valgrind. Might contain large ouput, which will be directed to data
./fuzzquery $indx 20 277 | ./querier $pdir $indx > ../data/test20_277
./fuzzquery $indx 1000 13904 | ./querier $pdir $indx > ../data/test1000_13904
$myvalgrind ./fuzzquery $indx 2000 240874 | ./querier $pdir $indx > ../data/valgrind2000_240874
./fuzzquery ../data/wiki2.index 5000 7025 | ./querier ../data/wiki-2 ../data/wiki2.index > ../data/test5000_7025
