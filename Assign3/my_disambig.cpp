#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <stdint.h>
#include "Ngram.h"

using namespace std;	

static Vocab Vocabulary;

static void LanguageModel_init(const char* lgmodel, int order);
static VocabIndex find(const char* w);
static double getWordProb2(VocabIndex wid1, VocabIndex wid2);
static double getWordProb3(VocabIndex wid1, VocabIndex wid2, VocabIndex wid3);
static void Viterbi_init(int num, const uint16_t* data);
static void Viterbi_iter2(int num, const uint16_t* data);
static void Viterbi_iter3(int num, const uint16_t* data);
static vector<uint16_t> Viterbi_back();

int main(int argc, char *argv[]){
	const char* order = argv[8];
	const char* lgmodel = argv[6];
	const char* mapping = argv[4];
	const char* testing = argv[2];
	int ngram_order = atoi(order);

	LanguageModel_init(lgmodel, ngram_order);
	map<uint16_t, vector<uint16_t> > mapdata;

	// build table
	ifstream mapfile(mapping);

	for (string line; getline(mapfile, line);){
		// find the key
		uint16_t key = (((uint16_t)line[0] & 255) << 8) + ((uint16_t)line[1] & 255);
		
		/*
		// test the key
		char tst1[3];
		tst1[0] = (char)line[0];
		tst1[1] = (char)line[1];
		tst1[2] = '\0';
		printf("%s\n", tst1);
		*/

		// find the value
		int len = line.size();
		vector<uint16_t> value;
		for (int v = 3; v < len;){
			while (isspace(line[v])){v ++; continue;}
			uint16_t tmp = (((uint16_t)line[v] & 255) << 8) + ((uint16_t)line[v + 1] & 255);

			/*
			char tst2[3];
			tst2[0] = (char)line[v];
			tst2[1] = (char)line[v + 1];
			tst2[2] = '\0';
			printf("%s\n", tst2);
			*/

			value.push_back(tmp);
			v += 2;
		}
		mapdata[key] = value;
	}
	mapfile.close();
	fprintf(stderr, "[Done] Successfully build table!\n");

	ifstream tstfile(testing);

	for (string line; getline(tstfile, line);){
		// calculate Viterbi array
		int o = 0;
		int len = line.size();
		for (int c = 0; c < len - 3; o ++){
			while (isspace(line[c])){c ++; continue;}
			uint16_t tmp = (((uint16_t)line[c] & 255) << 8) + (((uint16_t)line[c + 1] & 255));
			
			/*
			char tst3[3];
			tst3[0] = (char)((tmp >> 8) & 255);
			tst3[1] = (char)(tmp & 255);
			tst3[2] = '\0';
			printf("%s\n", tst3);
			*/

			vector<uint16_t> &data = mapdata[tmp];
			if (o == 0) Viterbi_init(data.size(), &data.front());
			else if (o == 1) Viterbi_iter2(data.size(), &data.front());
			else if (ngram_order == 2) Viterbi_iter2(data.size(), &data.front());
			else if (ngram_order == 3) Viterbi_iter3(data.size(), &data.front());
			
			c += 2;
		}
		fprintf(stderr, "[Done] Successfully calculate for Viterbi array!\n");

		// Find Viterbi backtrack path
		vector<uint16_t> list = Viterbi_back();
		cout << "<s> ";
		int sze = list.size();
		for (int k = 0; k < sze; k ++){
			uint16_t ans = list[k];
			char ans_print[3];
			ans_print[0] = (char)((ans >> 8) & 255);
			ans_print[1] = (char)(ans & 255);
			ans_print[2] = '\0';
			cout << ans_print << " ";
		}
		cout << "</s>" << "\n";
		fprintf(stderr, "[Done] Successfully find the Viterbi backtrack path!\n\n");
	}
}


/* ================== Language Model =================== */

static Ngram *Model = NULL;

static void LanguageModel_init(const char* lgmodel, int order){
	Model = new Ngram(Vocabulary, order);

	File lgdata(lgmodel, "r");
	Model->read(lgdata);
	lgdata.close();
}

static VocabIndex find(const char* w){
	VocabIndex wid = Vocabulary.getIndex(w);
	if (wid == Vocab_None)
		wid = Vocabulary.getIndex(Vocab_Unknown);
	return wid;
}

// bigram
static double getWordProb2(VocabIndex wid1, VocabIndex wid2){
	VocabIndex context[] = {wid1, Vocab_None};
	return Model->wordProb(wid2, context);
}

// trigram
static double getWordProb3(VocabIndex wid1, VocabIndex wid2, VocabIndex wid3){
	VocabIndex context[] = {wid2, wid1, Vocab_None};
	return Model->wordProb(wid3, context);
}


/* ================= Vierbi Algorithm ================== */
map<uint16_t, double> delta[3];
vector<map<uint16_t, uint16_t> > psi;

static void Viterbi_init(int num, const uint16_t* data){
	delta[0].clear();
	delta[1].clear();
	delta[2].clear();
	psi.clear();

	for (int k = 0; k < num; k ++){
		uint16_t word = data[k];

		/*
		char tst4[3];
		tst4[0] = (char)((word >> 8) & 255);
		tst4[1] = (char)(word & 255);
		tst4[2] = '\0';
		printf("%s\n", tst4);
		*/

		delta[0][word] = 0.0; // Log Prob
	}
}

static void Viterbi_iter2(int num, const uint16_t* data){
	delta[1].clear();
	
	psi.push_back(map<uint16_t, uint16_t>());
	
	// iteration for num of possible words in ZhuYin.map
	for (int k = 0; k < num; k ++){
		uint16_t word = data[k];

		/*
		char tst5[3];
		tst5[0] = (char)((word >> 8) & 255);
		tst5[1] = (char)(word & 255);
		tst5[2] = '\0';
		printf("%s\n", tst5);
		*/

		char w[3];
		w[0] = (char)((word >> 8) & 255);
		w[1] = (char)(word & 255);
		w[2] = '\0';
		VocabIndex wid = find(w);

		// calculate the possibilities
		double max_prob = -1.0 / 0.0;
		uint16_t max_prob_word = 0;
		// it->first  is the  key  of the map
		// it->second is the value of the map
		map<uint16_t, double>::iterator it = delta[0].begin(),
										it_end = delta[0].end();
		for (; it != it_end; it ++){
			uint16_t pre_word = it->first;
			char pre_w[3];
			pre_w[0] = (char)((pre_word >> 8) & 255);
			pre_w[1] = (char)(pre_word & 255);
			pre_w[2] = '\0';
			VocabIndex pre_wid = find(pre_w);

			double pre_prob = it->second;
			LogP prob = getWordProb2(pre_wid, wid);

			if (prob == LogP_Zero) continue;

			if (pre_prob + prob > max_prob){
				max_prob = pre_prob + prob;
				max_prob_word = pre_word;
			}
		}

		if (max_prob_word != 0){
			delta[1][word] = max_prob;
			psi.back()[word] = max_prob_word;
		}
	}

	swap(delta[0], delta[1]);
}
static void Viterbi_iter3(int num, const uint16_t* data){
	delta[2].clear();
	
	psi.push_back(map<uint16_t, uint16_t>());
	
	// iteration for num of possible words in ZhuYin.map
	for (int k = 0; k < num; k ++){
		uint16_t word = data[k];

		/*
		char tst5[3];
		tst5[0] = (char)((word >> 8) & 255);
		tst5[1] = (char)(word & 255);
		tst5[2] = '\0';
		printf("%s\n", tst5);
		*/

		char w[3];
		w[0] = (char)((word >> 8) & 255);
		w[1] = (char)(word & 255);
		w[2] = '\0';
		VocabIndex wid = find(w);

		// calculate the possibilities
		double max_prob = -1.0 / 0.0;
		uint16_t max_prob_word = 0;
		// it->first  is the  key  of the map
		// it->second is the value of the map
		map<uint16_t, double>::iterator it1 = delta[0].begin(),
										it1_end = delta[0].end();
		map<uint16_t, double>::iterator it2 = delta[1].begin(),
										it2_end = delta[1].end();
		for (; it1 != it1_end; it1 ++){
			uint16_t pre1_word = it1->first;
			char pre1_w[3];
			pre1_w[0] = (char)((pre1_word >> 8) & 255);
			pre1_w[1] = (char)(pre1_word & 255);
			pre1_w[2] = '\0';
			VocabIndex pre1_wid = find(pre1_w);

			double pre1_prob = it1->second;

			for (; it2 != it2_end; it2 ++){
				uint16_t pre2_word = it2->first;
				char pre2_w[3];
				pre2_w[0] = (char)((pre2_word >> 8) & 255);
				pre2_w[1] = (char)(pre2_word & 255);
				pre2_w[2] = '\0';
				VocabIndex pre2_wid = find(pre2_w);

				double pre2_prob = it2->second;
				LogP prob = getWordProb3(pre1_wid, pre2_wid, wid);

				if (prob == LogP_Zero) continue;

				if (pre1_prob + pre2_prob + prob > max_prob){
					max_prob = pre1_prob + pre2_prob + prob;
					max_prob_word = pre2_word;
				}
			}
		}

		if (max_prob_word != 0){
			delta[2][word] = max_prob;
			psi.back()[word] = max_prob_word;
		}
	}

	swap(delta[0], delta[1]);
	swap(delta[1], delta[2]);
}

static vector<uint16_t> Viterbi_back(){
	double max_prob = -1.0 / 0.0;
	uint16_t ans_word = 0;
	map<uint16_t, double>::iterator it = delta[0].begin(),
									it_end = delta[0].end();

	for (; it != it_end; it ++){
		uint16_t word = it->first;
		
		/*
		char tst6[3];
		tst6[0] = (char)((word >> 8) & 255);
		tst6[1] = (char)(word & 255);
		tst6[2] = '\0';
		printf("%s\n", tst6);
		*/

		double prob = it->second;
		if (prob > max_prob) ans_word = word, max_prob = prob;
	}

	vector<uint16_t> list;
	list.push_back(ans_word);

	while (!psi.empty()){
		ans_word = psi.back()[ans_word];
		psi.pop_back();
		list.push_back(ans_word);
	}
	reverse(list.begin(), list.end());
	return list;
}
