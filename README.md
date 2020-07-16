# WordSeg
English language word segmentation on strings with no spaces in a Header Only Library (WordSeg.hpp).

Based on Chapter 14 of Beautiful Data and using the word unigram frequencies from Peter Norvig: https://norvig.com/ngrams/

This is essentially a C++ port of his segment() function. Both the system() calls with Python one liners and Pythons C API were too slow to be useful.

This uses his unigram model and memoized recursive function design. I implemented simple but effective memoization via an unordered_map.

Included is a simple test program and count_1w.txt which contains the English letter counts distilled from the Google Web Trillion Word Corpus.
