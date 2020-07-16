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
	WordSeg model("count_1w.txt");
	WSUnigram bar;
	vector<WSUnigram> ex;
	string test;

	
	test = model.str_upper("tobeornottobe");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = model.str_upper("wheninthecourse");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = model.str_upper("wheninthecourseof");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = model.str_upper("wheninthecourseofhumanevents");
	ex = model.Segment(model.str_upper(test));
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	test = model.str_upper("essciftfooaanacfnowwswdp");
	ex = model.Segment(model.str_upper(test));
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = model.str_upper("inaholeinthegroundtherelivedahobbitnotanastydirtywetholefilledwiththeendsofwormsandanoozysmellnoryetadrybaresandyholewithnothinginittositdownonortoeatitwasahobbitholeandthatmeanscomfort");
	ex = model.Segment(model.str_upper(test));
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = model.str_upper("MostlockingroutinessuchaspthreadssemaphoresandsysvsemaphoresworkonthatsortoflogicalthoughthespecificAPIcallsaredifferent");
	ex = model.Segment(model.str_upper(test));
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	
	test = model.str_upper("ThethousandinjuriesofFortunatoIhadborneasIbestcouldbutwhenheventureduponinsultIvowedrevengeYouwhosowellknowthenatureofmysoulwillnotsupposehoweverthatIgaveutterancetoathreat");
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	time_t before;
	time(&before);
	
	test = "IIOASAINTTICFTDOCTSATNTOIICBOBSRAATAOOFSWOMSTSJTMDAASUIFIIGOSSWKSAIEABBDHSGSBTTUTIAOOEWRCYTSHCCAOSIDINTSISTCTITTTMCGAWTTABIENIBSBSEHOENTOSTTAOOTTITOATUOIICMWOIACSACFIICTCOATSEIBISDTSSVNOEUWFHMOTSTBIITSINWTIONAIOIACCTOASINNISIOOFAMPTETSTDSIOATITOSIASMSAOAPTICTHOONMTTOTNSTOMMANAWIDISONOTATOBCDTISIYFAOSDETBATITTSISKTOWHSECATOSAHDITHCFNOOTSSFOTIOETFHITTIASPTTINSTOOTOONWIIPDSARYIDPDISOTINTIAFEADIPCRTMWDSACAOIETSSOISTIITNHAESADATSCITFMTTSASTTSTUBSOANIHSICBLTWFTTOPSACTSHTOOHDWOSBTIOCSDIFWWONTSMONTBWIITOUIIIITSIIWOCWTIOIIA";
	ex = model.Segment(test);
	
	for(int i = 0; i < (int)ex.size(); i++) {
		cout << ex[i].gram << " ";
	}
	cout << endl << endl;
	//cout << model.GetNumIters() << " for size " << test.size() << ". segMemo size: " << model.GetMemoSize() << endl << endl;
	
	time_t after;
	time(&after);
	
	cout << "Time Taken on the last test: " << after - before << " seconds." << endl << endl;
	
	
	return 0;
}

