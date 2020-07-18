/*
 * WordSeg.hpp
 * 
 * Copyright 2020 gjorm <jakemhaskins@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifndef WORDSEG_HPP
#define WORDSEG_HPP

#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <array>
#include <memory>
#include <cctype>

using namespace std;

class WSGram
{
private:
	
		
public:
	string gram;
	double score;
	
	WSGram() {
		gram = "";
		score = 0.0;
	}
	
	WSGram(string str, double sc) {
		gram = str;
		score = sc;
	}
	
	//Getters
	string GetGram() {
		return gram;
	}
	
	double GetScore() {
		return score;
	}
	
	//Setters
	void SetGram(string str) {
		gram = str;
	}
	
	void SetScore(double s) {
		score = s;
	}
};

bool operator < (const WSGram &lhs, const WSGram &rhs) {
	return lhs.score < rhs.score;
}



class WordSeg
{
private:
	unordered_map<string, WSGram> uniGrams;
	unordered_map<string, WSGram> biGrams;
	unordered_map<string, vector<WSGram>> segMemo;//a hash map to memoize Segment()
	const double numCounts = 1024908267229;
	double gMin = 1000000000.0, gMax = 0.0;
	double bgMin = 1000000000.0, bgMax = 0.0;
	const int maxSegLength = 24;
	const int segMemoSize = 10000;//arbitrary initial size for segMemo
	int numIters = 0;
	
	//check for good ascii and numeric characters
	bool IsNumeric(char c) {
		if (isascii(c)) {
			if (IsInteger(c) || c == '.')
				return true;
		}

		return false;
	}
	
	bool IsDelimiter(char c) {
		if(c == ' ' || c == '\n' || c == '\t')
			return true;
		
		return false;
	}

	//check for integral value
	bool IsInteger(char c) {
		if (isascii(c)) {
			if ((c >= '0' && c <= '9'))
				return true;
		}

		return false;
	}

	//determine if purely Alphabetic
	bool IsAlphabetic(char c) {
		if (isascii(c)) {
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
				return true;
		}

		return false;
	}
	
	//Private inner recursive method of Segment
	//Segment
	vector<WSGram> pSegment(const string &in) {
		WSGram wsLeft, wsRight;
		vector<WSGram> cands;
		vector<WSGram> result;
		priority_queue<vector<WSGram>> candHeap;
		WSGram def;
		unordered_map<string, vector<WSGram>>::iterator mi;
		
		def.gram = "";
		def.score = GetGramScore("");
		
		if(in == "") {
			result.push_back(def);
			return result;
		}
		else {//"memoize" section, dont do the work if the input already exists in the seg memo hash table
			mi = segMemo.find(in);
			if(mi != segMemo.end()) {
				result = mi->second;
				return result;
			}
			else {
				for(int i = 1; (i <= (int)in.size() && i <= maxSegLength); i++) {
					//split the text
					wsLeft.gram = in.substr(0, i);
					wsRight.gram = in.substr(i, string::npos);
					
					//recursively call pSegment on the right string
					cands = pSegment(wsRight.gram);
					
					//Score
					//only need to score the left gram; the lower recursive depths will return this and higher depths will accept as the higher prob segment of right
					wsLeft.score = GetGramScore(wsLeft.gram);
					
					//store into result (result is being used as a worker, not a return val yet)
					result.push_back(wsLeft);
					result.insert(result.end(), cands.begin(), cands.end());
					
					//GetVecBiGramScore(result);
					
					//push onto the priority queue
					candHeap.push(result);
					
					//clear out these vectors for next iteration
					result.clear();
					cands.clear();
					
					//this is commented out for now, but used for troubleshooting
					numIters++;
				}
			}
			
				
			//grab the highest valued WSGram attempt from the priority_queue
			result = candHeap.top();
			
			//insert the vector into the segMemo hash table, as it was not found at the beginning of the method
			segMemo.insert(make_pair(in, result));

		}
		
		return result;
	}

	
public:
	WordSeg(const char *uniGramFile, const char *biGramFile) {
		ifstream inFile;
		unordered_map<string, WSGram>::iterator gi;
		pair<unordered_map<string, WSGram>::iterator, bool> res;
		
		//File Input for Unigrams
		inFile.open(uniGramFile);
		if(inFile.is_open()) {
			char c;
			WSGram foo;
			string get = "";
			bool alt = true;

			c = inFile.get();
			while (inFile.good() && !inFile.eof()) {

				if (IsAlphabetic(c) || IsNumeric(c)) {
					get += towupper(c);
				}
				else {
					if (alt) {//first portion in the line is the gram
						foo.SetGram(get);
						alt = false;
						get = "";
					}
					else {//second is the score.
						foo.SetScore(stod(get) / numCounts); //store raw counts
						alt = true;
						get = "";
						
						//make the insert after the score is found
						//attempt the insert
						res = uniGrams.insert(make_pair(foo.GetGram(), foo));
						//if unique, the WSGram will successfully insert into unordered_map, if not, then there are duplicates in the file so add the found count with the existing count
						if (!res.second) {
							gi = res.first;
							gi->second.score += foo.GetScore();
						}
						
						//update minima and maxima
						if(gMin > foo.GetScore()) gMin = foo.GetScore();
						if(gMax < foo.GetScore()) gMax = foo.GetScore();
					}
				}

				c = inFile.get();//put this last as eof will not be reached until eof character is extracted from stream
			}
		}
		else {
			cout << "WordSeg could not open the file for parsing: " << uniGramFile << endl;
			return;
		}
		inFile.close();
		
		
		//File Input for Bi Grams
		inFile.open(biGramFile);
		if(inFile.is_open()) {
			WSGram foo;
			string get = "";
			int alt = 0;
			char c;

			while (inFile.good()) {
				c = inFile.get();
				
				if (!IsDelimiter(c) && !inFile.eof()) {
					get += towupper(c);
				}
				else {
					if (alt == 0) {//first two portions in the line is the gram
						foo.SetGram(get);
						alt++;
						get = "";
					}
					else if(alt == 1) {
						foo.SetGram(foo.GetGram() + " " + get);
						alt++;
						get = "";
						
					}
					else if(alt == 2) {//last is the score.
						foo.SetScore(stod(get) / numCounts); //store the raw counts
						alt = 0;
						get = "";
						
						//make the insert after the score is found
						//attempt the insert
						res = biGrams.insert(make_pair(foo.GetGram(), foo));
						//if unique, the WSGram will successfully insert into unordered_map, if not, then there are duplicates in the file so add the found count with the existing count
						if (!res.second) {
							gi = res.first;
							gi->second.score += foo.GetScore();
						}
						
						//update minima and maxima
						if(bgMin > foo.GetScore()) bgMin = foo.GetScore();
						if(bgMax < foo.GetScore()) bgMax = foo.GetScore();
					}
				}
			}
		}
		else {
			cout << "WordSeg could not open the file for parsing: " << biGramFile << endl;
			return;
		}
		inFile.close();
		
		//Set a large bucket size for the Memo Hash Table
		segMemo.rehash(segMemoSize);

	}
	
	~WordSeg() {
		uniGrams.clear();
		biGrams.clear();
		segMemo.clear();
	}
	

	//Public Accessible Segment function for managing the segMemo hash table
	vector<WSGram> Segment(const string &in) {
		vector<WSGram> result = pSegment(in);

		//may need to comment this out during a HillClimb() then call ClearSegMemo() after a more ideal period as
		//segMemo will maintain return values that benefit other runs of Segment, even with different input
		//segMemo.clear();
		numIters = 0;
		
		return result;
	}
	
	void ClearSegMemo() {
		segMemo.clear();
	}
	
	
	
	//Getters
	
	//calculate score given a single word gram
	double GetGramScore(const string &in) {
		unordered_map<string, WSGram>::iterator gi;
		double result = 0.0;
		
		if(in.size() <= 0 || in == "") {
			result = log10(1.0);
		}
		else {
			gi = uniGrams.find(in);
			
			if (gi != uniGrams.end()) {
				result = log10(gi->second.GetScore());
			}
			else {
				result = log10(gMin - (int)in.size());
			}
		}

		return result;
	}
	
	//calculate a vector of grams' scores
	double GetVecGramScore(vector<WSGram> &vG) {
		double total = 0.0;
		for(int i = 0; i < (int)vG.size(); i++) {
			vG[i].score = GetGramScore(vG[i].gram);
			total += vG[i].score;
		}
		
		return total;
	}
	
	//retrieve a vector of grams as a string
	string GetVecGram(const vector<WSGram> &vG) {
		string result = "";
		
		for(int i = 0; i < (int)vG.size(); i++) {
			result += vG[i].gram + " ";
		}
		
		return result;
	}
	
	//retrieve a vector of grams consolidated into a pair with the sum of its scores and a string of its concatenated grams
	pair<double, string> GetVecPair(vector<WSGram> &vG) {
		pair<double, string> result;
		
		for(int i = 0; i < (int)vG.size(); i++) {
			result.first += vG[i].score;
			result.second += vG[i].gram + " ";
		}
		
		return result;
	}
	
	//get the raw count (no log10 or division by numCounts) of a gram
	double GetGramRawCount(const string &in) {
		double result;
		unordered_map<string, WSGram>::iterator gi;
		
		if(in.size() <= 0 || in == "") {
			result = 0;
		}
		else {
			gi = uniGrams.find(in);
			
			if (gi != uniGrams.end()) {
				result = gi->second.GetScore();
			}
			else {
				result = (gMin / 2) - (int)in.size();
			}
		}
		
		return result;
	}
	
	//calculate score given a word-biGram
	double GetBiGramScore(const string &left, const string &right) {
		unordered_map<string, WSGram>::iterator gi;
		double result = 0.0;
		string test = left + " " + right;
		
		if(test.size() <= 0 || test == "") {
			result = log10(1.0 / numCounts);
		}
		else {
			gi = biGrams.find(test);
			
			if (gi != uniGrams.end()) {
				result = log10(gi->second.GetScore());
			}
			else {
				//on failure to find anything in the bigrams, fall back to unigrams for the right string
				result += GetGramScore(right);
			}
		}

		return result;
	}
	
	//score a vector of grams using biGram data
	double GetVecBiGramScore(vector<WSGram> &vec) {
		double result;
		
		vec[0].score = GetGramScore(vec[0].gram);
		
		result = vec[0].score;
		
		for(int i = 1; i < (int)vec.size(); i++) {
			vec[i].score = GetBiGramScore(vec[i - 1].gram, vec[i].gram);
			result += vec[i].score;
		}
		
		return result;
	}
	
	//convert an entire string to upper case
	string StringUpper(const string &in) {
		string result;
		
		for(int i = 0; i < (int)in.size(); i++) {
			result += towupper(in[i]);
		}
		
		return result;
	}
	
	//regular accessors
	int GetNumGrams() {
		return (int)uniGrams.size();
	}
	
	int GetNumIters() {
		int result = numIters;
		numIters = 0;
		return result;
	}
	
	int GetMemoSize() {
		return segMemo.size();
	}
};

//operator overload for the priority queue of a vector of grams
bool operator < (const vector<WSGram> &lhs, const vector<WSGram> &rhs) {
	double lScore = 0;
	double rScore = 0;
	
	for(int i = 0; i < (int)lhs.size(); i++) {
		lScore += lhs[i].score;
	}
	
	for(int i = 0; i < (int)rhs.size(); i++) {
		rScore += rhs[i].score;
	}
	
	return lScore < rScore;
}

#endif
