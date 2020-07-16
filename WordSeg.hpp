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

class WSUnigram
{
private:
	
		
public:
	string gram;
	double score;
	
	WSUnigram() {
		gram = "";
		score = 0.0;
	}
	
	WSUnigram(string str, double sc) {
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

bool operator < (const WSUnigram &lhs, const WSUnigram &rhs) {
	return lhs.score < rhs.score;
}



class WordSeg
{
private:
	unordered_map<string, WSUnigram> uniGrams;
	unordered_map<string, vector<WSUnigram>> segMemo;//a hash map to memoize Segment()
	const double numCounts = 1024908267229;
	double gMin = 1000000000.0, gMax = 0.0;
	const int maxSegLength = 24;
	const int segMemoSize = 10000;//initial size for segMemo
	int numIters = 0;
	
	//check for good ascii and numeric characters
	bool IsNumeric(char c) {
		if (isascii(c)) {
			if (IsInteger(c) || c == '.')
				return true;
		}

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
	
	//Private inner method of Segment to manage segMemo
	//Segment
	vector<WSUnigram> pSegment(const string &in) {
		WSUnigram wsLeft, wsRight;
		vector<WSUnigram> cands;
		vector<WSUnigram> result;
		priority_queue<vector<WSUnigram>> candHeap;
		WSUnigram def;
		unordered_map<string, vector<WSUnigram>>::iterator mi;
		
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
					
					//score
					wsLeft.score = GetGramScore(wsLeft.gram);
					GetVecGramScore(cands);
					
					//store into result (result is being used as a worker, not a return val yet)
					result.push_back(wsLeft);
					result.insert(result.end(), cands.begin(), cands.end());
					
					//push onto the priority queue
					candHeap.push(result);
					
					//clear out these vectors for next iteration
					result.clear();
					cands.clear();
					
					//this is commented out for now, but used for troubleshooting
					//numIters++;
				}
			}
			
				
			//grab the highest valued WSUniGram attempt from the priority_queue
			result = candHeap.top();
			
			//insert the vector into the segMemo hash table, as it was not found at the beginning of the method
			segMemo.insert(make_pair(in, result));

		}
		
		return result;
	}

	
public:
	WordSeg(const char* fileName) {
		ifstream inFile;
		inFile.open(fileName);
		if(inFile.is_open()) {
			char c;
			WSUnigram foo;
			string get = "";
			bool alt = true;

			c = inFile.get();
			while (inFile.good()) {

				if (IsAlphabetic(c) || IsNumeric(c)) {
					get += towupper(c);
				}
				else {
					if (alt) {//first portion int he line is the gram
						foo.SetGram(get);
						alt = false;
						get = "";
					}
					else {//second is the score.
						foo.SetScore(stod(get) / numCounts); //make sure to divide by numCounts and that low prob grams are 1 / numCounts in other code
						alt = true;
						get = "";
						
						//make the insert after the score is found
						uniGrams.insert(make_pair(foo.GetGram(), foo));
						
						//update minima and maxima
						if(gMin > foo.GetScore()) gMin = foo.GetScore();
						if(gMax < foo.GetScore()) gMax = foo.GetScore();
					}
				}

				c = inFile.get();//put this last as eof will not be reached until eof character is extracted from stream
			}
		}
		else {
			cout << "WordSeg could not open the file for parsing: " << fileName << endl;
			return;
		}
		
		segMemo.rehash(segMemoSize);

	}
	
	~WordSeg() {
		uniGrams.clear();
		segMemo.clear();
	}
	

	//Public Accessible Segment function
	vector<WSUnigram> Segment(const string &in) {
		vector<WSUnigram> result = pSegment(in);

		//may need to comment this out during a HillClimb() then call ClearSegMemo() after each restart
		segMemo.clear();
		
		return result;
	}
	
	void ClearSegMemo() {
		segMemo.clear();
	}
	
	
	
	//Getters
	
	//calculate score given a single word gram
	double GetGramScore(const string &in) {
		unordered_map<string, WSUnigram>::iterator gi;
		double result = 0.0;
		string test = "";
		
		if(in.size() <= 0 || in == "") {
			result = log10(1.0 / numCounts);
		}
		else {
			test = str_upper(in);
			gi = uniGrams.find(test);
			
			if (gi != uniGrams.end()) {
				result = log10(gi->second.GetScore());
			}
			else {
				result = log10(gMin - (int)in.size());
			}
		}

		return result;
	}
	
	void GetVecGramScore(vector<WSUnigram> &vG) {
		
		for(int i = 0; i < (int)vG.size(); i++) {
			vG[i].score = GetGramScore(vG[i].gram);
		}
		
		return;
	}
	
	string str_upper(const string &in) {
		string result;
		
		for(int i = 0; i < (int)in.size(); i++) {
			result += towupper(in[i]);
		}
		
		return result;
	}
	
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


bool operator < (const vector<WSUnigram> &lhs, const vector<WSUnigram> &rhs) {
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
