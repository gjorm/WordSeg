# WordSeg
Word Segmentation on strings with no spaces in a Header Only Library (WordSeg.hpp).

Based on Chapter 14 of Beautiful Data and using the word unigram frequencies from Peter Norvig: https://norvig.com/ngrams/

This is essentially a C++ port of his WordSegment module (https://pypi.org/project/wordsegment/). I wanted speed and both the system() calls to Python one-liners and Pythons C API calls to WordSegment.segment() were too slow to be useful, even if the module was instantiated once at runtime, when used in an inner loop testing several iterations.

This uses his unigram model and memoized recursive function design. I implemented simple but effective memoization via an unordered_map.

Included is support for BiGrams, so you can score a vector of grams with the BiGram data, however Segment() still uses unigrams in its scoring method as BiGrams is much slower and yields improvements in accuracy in some areas but worse in others.

Included is a simple test program (main.cpp), count_1w.txt, and count_2w.txt which contains the English word uni/biGram counts distilled from the Google Web Trillion Word Corpus.
