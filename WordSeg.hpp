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
#include <cfloat>

using namespace std;

class WSGram
{
private:
	
		
public:
	string gram;
	double score;
	
	WSGram() {
		this->gram = "";
		this->score = 0.0;
	}
	
	WSGram(string str, double sc) {
		this->gram = str;
		this->score = sc;
	}
	
	//Getters
	string GetGram() {
		return this->gram;
	}
	
	double GetScore() {
		return this->score;
	}
	
	//Setters
	void SetGram(string str) {
		this->gram = str;
	}
	
	void SetScore(double s) {
		this->score = s;
	}
};

//Operator Overloads
bool operator < (const WSGram &lhs, const WSGram &rhs) {
	return lhs.score < rhs.score;
}



class WordSeg
{
private:
	unordered_map<string, WSGram> uniGrams;
	unordered_map<string, WSGram> biGrams;
	unordered_map<string, pair<double, vector<WSGram>>> segMemo;//a hash map to memoize Segment()
	double numCounts = 0;// 494290;
	double numCounts2 = 0;
	double gMin = 10000000000.0, gMax = 0.0;
	double bgMin = 10000000000.0, bgMax = 0.0;
	const int maxSegLength = 18;//Norvig uses 24 characters for this, but I've tuned it down to 18 for speed
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
	pair<double, vector<WSGram>> pSegment(const string &in) {
		WSGram wsLeft, wsRight;
		//using pair<double, vector<WSGGram>> for all storage needs, so that scoring happens only once. No need to reiterate over the vector of grams over and over
		pair<double, vector<WSGram>> cands;
		pair<double, vector<WSGram>> result;
		vector<pair<double, vector<WSGram>>> candHeap;
		unordered_map<string, pair<double, vector<WSGram>>>::iterator mi;
		
		if(in == "") {
			result.second.push_back(WSGram("", GetGramScore("")));
			result.first = GetGramScore("");
			return result;
		}
		else {//"memoize" section; dont do the work if the input already exists in the segMemo hash table
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
					result.second.push_back(wsLeft);
					//store right side cands
					if(cands.second[cands.second.size() - 1].GetGram() != "") result.second.insert(result.second.end(), cands.second.begin(), cands.second.end());
					//grab scores of the left Gram plus the right side vector of candidates
					result.first = wsLeft.score + cands.first;
					
					//push onto the candHeap vector...
					if(!result.second.empty()) candHeap.push_back(result);
					
					//clear out these vectors for next iteration
					result.second.clear();
					result.first = 0.0;
					cands.second.clear();
					cands.first = 0.0;
					
					//count the number of iterations it takes to compute an input
					numIters++;
				}
				
				//grab the highest valued WSGram attempt from the candHeap vector
				//Iterating through the vector afterwards is a touch faster than a priority_queue during the above for loop, for the string size of a few sentences anyways
				result = candHeap[0];
				
				
				for(int i = 1; i < (int)candHeap.size(); i++) {
					
					if(result.first < candHeap[i].first) {
						result = candHeap[i];
					}
				}
				
				
				
				//insert the vector into the segMemo hash table, as it was not found at the beginning of the method
				segMemo.insert(make_pair(in, result));
			}
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
			while (inFile.good()) {

				if (!IsDelimiter(c) && !inFile.eof()) {
					get += towupper(c);
				}
				else {
					if (alt) {//first portion in the line is the gram
						foo.SetGram(get);
						alt = false;
						get = "";
					}
					else {//second is the score.
						foo.SetScore(stod(get)); //store raw counts
						alt = true;
						get = "";
						
						//update numCounts
						numCounts += (double)foo.GetScore();
						
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
						foo.SetScore(stod(get)); //store the raw counts
						alt = 0;
						get = "";
						
						//update numCounts
						numCounts2 += (double)foo.GetScore();
						
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
		
		
		//Set an arbitarily large bucket size for the Memo Hash Table
		segMemo.rehash(segMemoSize);

	}
	
	~WordSeg() {
		uniGrams.clear();
		biGrams.clear();
		segMemo.clear();
	}
	
	void PrintSegMemo() {
		unordered_map<string, pair<double, vector<WSGram>>>::iterator gi;
		vector<WSGram> gram;
		string input;
		
		if(segMemo.size() > 0) {
			for(gi = segMemo.begin(); gi != segMemo.end(); ++gi) {
				input = gi->first;
				cout << "    Input: " << input << endl;
				gram = gi->second.second;
				for(int i = 0; i < (int)gram.size(); i++) {
					cout << gram[i].gram << " " << gram[i].score << endl;
				}
				cout << endl;
			}
		}
	}
	

	//Public Accessible Segment function for managing the segMemo hash table
	vector<WSGram> Segment(const string &in) {

		pair<double, vector<WSGram>> result = pSegment(in);

		//may need to comment this out during a HillClimb() then call ClearSegMemo() after a more ideal period as
		//segMemo will maintain return values that benefit other runs of Segment, even with different input
		segMemo.clear();
		
		return result.second;
	}

	//Public Accessible Segment function for managing the segMemo hash table, but returns the score in a pair with the vector results
	pair<double, vector<WSGram>> SegmentRetScore(const string &in) {

		pair<double, vector<WSGram>> result = pSegment(in);

		//may need to comment this out during a HillClimb() then call ClearSegMemo() after a more ideal period as
		//segMemo will maintain return values that benefit other runs of Segment, even with different input
		segMemo.clear();
		
		return result;
	}

	//Public Accessible Segment function for managing the segMemo hash table, but does not require all caps input
	vector<WSGram> SegmentAnyCase(const string &in) {
		
		//capitalize the input into pSegment
		pair<double, vector<WSGram>> result = pSegment(StringUpper(in));

		//may need to comment this out during a HillClimb() then call ClearSegMemo() after a more ideal period as
		//segMemo will maintain return values that benefit other runs of Segment, even with different input
		segMemo.clear();
		
		return result.second;
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
			result = GetEmptyGramScore();
		}
		else {
			gi = uniGrams.find(in);
			
			//if the gram is found, find its raw counts and calculate its score
			if (gi != uniGrams.end()) {
				result = log10(gi->second.GetScore() / numCounts);// * (double)in.size();
			}
			else {//gram is not found
				result = log10((1 / (numCounts * 2))) * (double)in.size();
			}
		}

		return result;
	}
	
	static double GetEmptyGramScore() {
		return 0;
	}
	
	//calculate a vector of grams' scores. The input vector is assumed to have been segmented and each Gram contains its pre calculated score from Segment()
	static double GetVecGramScore(const vector<WSGram> &vG) {
		double total = GetEmptyGramScore();
		
		if(vG.size() > 0) {
			for(int i = 0; i < (int)vG.size(); i++) {
				total += vG[i].score;
			}
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
				result = (double)gMin - in.size();
			}
		}
		
		return result;
	}

	
	//calculate score given a word-biGram
	double GetBiGramScore(const string &left, const string &right) {
		unordered_map<string, WSGram>::iterator gi;
		double result = 0.0;
		
		//bigrams are space separated unigrams
		string test = left + " " + right;
		
		if(test == "") {
			result = 0;
		}
		else {
			gi = biGrams.find(test);
			
			if (gi != uniGrams.end()) {
				result = log10((gi->second.GetScore() * test.size()) / numCounts2) / GetGramScore(left);
			}
			else {
				//on failure to find anything in the bigrams, fall back to unigrams for the input strings
				result = GetGramScore(left);
				result += GetGramScore(right);
			}
		}

		return result;
	}
	
	//score a vector of grams using biGram data
	double GetVecBiGramScore(vector<WSGram> &vec) {
		double result;
		
		result = 0.0;
		
		if(vec.size() > 0) {
			vec[0].score = GetGramScore(vec[0].gram);
			
			result = vec[0].score;
			
			for(int i = 1; i < (int)vec.size(); i++) {
				vec[i].score = GetBiGramScore(vec[i - 1].gram, vec[i].gram);
				result += vec[i].score;
			}
		}
		
		return result;
	}

	//retrieve a vector of grams consolidated into a pair with the sum of its bigram scores and a string of its concatenated grams
	pair<double, string> GetVecBiGramPair(vector<WSGram> &vG) {
		pair<double, string> result;
		result.first = GetVecBiGramScore(vG);
		
		for(int i = 0; i < (int)vG.size(); i++) {
			result.second += vG[i].gram + " ";
		}
		
		return result;
	}
	
	//convert an entire string to upper case
	static string StringUpper(const string &in) {
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
	return WordSeg::GetVecGramScore(lhs) < WordSeg::GetVecGramScore(rhs);
}

#endif
