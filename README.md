# WordSeg
English language word segmentation on strings with no spaces in a Header Only Library (WordSeg.hpp).

Based on Chapter 14 of Beautiful Data and using the word unigram frequencies from Peter Norvig: https://norvig.com/ngrams/

This is essentially a C++ port of his segment() function. Both the system() calls to Python one-liners and Pythons C API were too slow to be useful.

This uses his unigram model and memoized recursive function design. I implemented simple but effective memoization via an unordered_map.

Included is support for BiGrams, so you can score a vector of grams with the BiGram data, however Segment() still uses unigrams in its scoring method.

Included is a simple test program (main.cpp), count_1w.txt, and count_2w.txt which contains the English word uni/biGram counts distilled from the Google Web Trillion Word Corpus.

Special Note:
  <b>You must supply WordSeg with strings entirely in upper case for it to work.</b>
