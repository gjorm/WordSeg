/*
 * main.cpp
 * 
 * Copyright 2020 Jake Haskins <jakemhaskins@gmail.com>
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


#include <iostream>
#include <string>
#include <ctime>
#include "WordSeg.hpp"

int main(int argc, char **argv)
{
	WordSeg model("count_1w.txt", "count_2w.txt");
	WSGram bar;
	vector<WSGram> ex;
	string test;
	double gramCount = 0;
	
	gramCount += model.GetGramScore("OOZY");
	cout << "Single Gram Proba Test for OOZY:  " << gramCount << endl;
	gramCount += model.GetGramScore("SMELL");
	cout << "Single Gram Proba Test for SMELL:  " << model.GetGramScore("SMELL") << endl;
	gramCount += model.GetGramScore("NOR");
	cout << "Single Gram Proba Test for NOR:  " << model.GetGramScore("NOR") << endl;
	gramCount += model.GetGramScore("YET");
	cout << "Single Gram Proba Test for YET: " << model.GetGramScore("YET") << endl;
	gramCount += model.GetGramScore("A");
	cout << "Single Gram Proba Test for A:  " << model.GetGramScore("A") << endl;
	gramCount += model.GetGramScore("DRY");
	cout << "Single Gram Proba Test for DRY:  " << model.GetGramScore("DRY") << endl;
	gramCount += model.GetGramScore("BARE");
	cout << "Single Gram Proba Test for BARE:  " << model.GetGramScore("BARE") << endl;
	cout << "Total: " << gramCount << endl << endl;
	
	cout << "Single Gram Proba Test for IIOASAINTTICFTDOCTSATN: " << model.GetGramScore("IIOASAINTTICFTDOCTSATN") << endl << endl;

	
	test = "IIOASAINTTICFTDOCTSATN";
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = WordSeg::StringUpper("wheninthecourse");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	//model.PrintSegMemo();
	//cout << endl;
	
	
	test = WordSeg::StringUpper("AFTERanunequivocalexperienceoftheinefficacyofthesubsistingfederalgovernmentyouarecalledupontodeliberateonanewConstitutionfortheUnitedStatesofAmericaThesubjectspeaksitsownimportancecomprehendinginitsconsequencesnothinglessthantheexistenceoftheUNIONthesafetyandwelfareofthepartsofwhichitiscomposedthefateofanempireinmanyrespectsthemostinterestingintheworldIthasbeenfrequentlyremarkedthatitseemstohavebeenreservedtothepeopleofthiscountrybytheirconductandexampletodecidetheimportantquestionwhethersocietiesofmenarereallycapableornotofestablishinggoodgovernmentfromreflectionandchoiceorwhethertheyareforeverdestinedtodependfortheirpoliticalconstitutionsonaccidentandforceIftherebeanytruthintheremarkthecrisisatwhichwearearrivedmaywithproprietyberegardedastheerainwhichthatdecisionistobemadeandawrongelectionofthepartweshallactmayinthisviewdeservetobeconsideredasthegeneralmisfortuneofmankind");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = WordSeg::StringUpper("wheninthecourseofhumanevents");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	test = "OOZYSMELLNORYETADRYBARE";
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = WordSeg::StringUpper("inaholeinthegroundtherelivedahobbitnotanastydirtywetholefilledwiththeendsofwormsandanoozysmellnoryetadrybaresandyholewithnothinginittositdownonortoeatitwasahobbitholeandthatmeanscomfort");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = WordSeg::StringUpper("MostlockingroutinessuchaspthreadssemaphoresandsysvsemaphoresworkonthatsortoflogicalthoughthespecificAPIcallsaredifferent");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = WordSeg::StringUpper("ThethousandinjuriesofFortunatoIhadborneasIbestcouldbutwhenheventureduponinsultIvowedrevengeYouwhosowellknowthenatureofmysoulwillnotsupposehoweverthatIgaveutterancetoathreat");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	//the following two examples have inputs with identical words, but in reverse order
	pair<double, string> exPair;
	test = WordSeg::StringUpper("backwardsissentencethis");
	ex = model.Segment(test);
	exPair = model.GetVecPair(ex);
	
	cout << exPair.second << endl;
	
	cout << "  uniGrams Score: " << exPair.first << endl;
	
	test = WordSeg::StringUpper("thissentenceisbackwards");
	ex = model.Segment(test);
	exPair = model.GetVecPair(ex);
	
	cout << exPair.second << endl;
	
	cout << "  uniGrams Score: " << exPair.first << endl;
	
	double biScore = model.GetVecBiGramScore(ex);
	
	cout << "  biGrams Score: " << biScore << endl << endl;
	
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	time_t before;
	time(&before);
	
	test = "IIOASAINTTICFTDOCTSATNTOIICBOBSRAATAOOFSWOMSTSJTMDAASUIFIIGOSSWKSAIEABBDHSGSBTTUTIAOOEWRCYTSHCCAOSIDINTSISTCTITTTMCGAWTTABIENIBSBSEHOENTOSTTAOOTTITOATUOIICMWOIACSACFIICTCOATSEIBISDTSSVNOEUWFHMOTSTBIITSINWTIONAIOIACCTOASINNISIOOFAMPTETSTDSIOATITOSIASMSAOAPTICTHOONMTTOTNSTOMMANAWIDISONOTATOBCDTISIYFAOSDETBATITTSISKTOWHSECATOSAHDITHCFNOOTSSFOTIOETFHITTIASPTTINSTOOTOONWIIPDSARYIDPDISOTINTIAFEADIPCRTMWDSACAOIETSSOISTIITNHAESADATSCITFMTTSASTTSTUBSOANIHSICBLTWFTTOPSACTSHTOOHDWOSBTIOCSDIFWWONTSMONTBWIITOUIIIITSIIWOCWTIOIIA";
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << "Score: " << model.GetVecGramScore(ex) << endl;
	cout << "Iterations: " << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	time_t after;
	time(&after);
	
	cout << "Time Taken on the last test: " << after - before << " seconds." << endl << endl;
	
	
	return 0;
}

