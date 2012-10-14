#include "Bayes.h"
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace std;

typedef TokenList::iterator TokenIter;
typedef Tokens::iterator TokensIter;
typedef TokenProb::iterator TokenPIter;

char* Bayes::delims = " .,;\"\'!@#$%^*(){}[]:|\\/?`~+<>";

Bayes::Bayes() : tm(0)
{
	spam = TokenList();
	ham = TokenList();
}

Bayes::Bayes(const char* dbfile) : tm(0)
{
	spam = TokenList();
	ham = TokenList();
	this->dbfile = string(dbfile);

	LoadDatabases(this->dbfile);
}

Bayes::Bayes(const string& dbfile) : tm(0)
{
	spam = TokenList();
	ham = TokenList();
	this->dbfile = dbfile;

	LoadDatabases(dbfile);
}

int Bayes::Categorize(const string& item)
{
	Tokens tokens;

	Tokenize(item, tokens);

	vector<double> probs;
	int i = 0;
	for(TokensIter it = tokens.begin(); it != tokens.end(); it++)
		if(prob.count(*it) > 0)
			probs.push_back(prob[*it]);
		else
			probs.push_back(0.4);

	sort(probs.begin(), probs.end());

	double mult = 1, comb = 1;
	i = 0;
	for(vector<double>::reverse_iterator it = probs.rbegin(); it != probs.rend(); it++)
	{
		double p = *it;
		mult *= p;
		comb *= (1 - p);
		if(++i > 15)
			break;
	}

	double result = mult / (mult + comb);
	if(result >= 0.9)
		return -1;
	else if(result >= 0.3)
		return 1;
	else
		return 0;
}

unsigned int Bayes::GetTokenCount(const string& token, BayesTokenType type)
{
	TokenList list = type == Spam ? spam : type == Ham ? ham : TokenList();
	if(list.count(token) == 0)
		return 0;
	return list[token];
}

void Bayes::CalcProbabilities(void)
{
	prob.clear();
	for(TokenIter it = ham.begin(); it != ham.end(); it++)
		CalcTokenProbability(it->first);
	for(TokenIter it = spam.begin(); it != spam.end(); it++)
		if(prob.count(it->first) == 0)
			CalcTokenProbability(it->first);
}

void Bayes::CalcTokenProbability(const string& token)
{
	int g = ham.count(token) != 0 ? GetTokenCount(token, Ham) * 2 : 0,
		b = spam.count(token) != 0 ? GetTokenCount(token, Spam) : 0;

	if((g + b) >= 5)
	{
		double gf = min(1, (double)g / (double)GetHamTokenCount() * 100),
			   bf = min(1, (double)b / (double)GetSpamTokenCount() * 100);

		double p = max(0.011, min(0.99, bf / (gf + bf)));

		if(g == 0)
			p = (b > 10) ? 0.9999 : 0.9998;

		prob[token] = p;
	}
}

void Bayes::Tokenize(const string& item, Tokens& tokens)
{
	char *str = _strdup(item.c_str()), *ctx = NULL;

	char* token = strtok_s(str, delims, &ctx);
	if(token != NULL) do { tokens.push_back(string(token)); } while((token = strtok_s(NULL, delims, &ctx)) != NULL);

	free(str);
}

void Bayes::AddSpam(const string& item, bool recalculate)
{
	time(&tm);
	Tokens tokens;
	Tokenize(item, tokens);
	for(TokensIter it = tokens.begin(); it != tokens.end(); it++)
		spam[*it]++;

	if(recalculate)
		CalcProbabilities();
}

void Bayes::AddHam(const string& item, bool recalculate)
{
	time(&tm);
	Tokens tokens;
	Tokenize(item, tokens);
	for(TokensIter it = tokens.begin(); it != tokens.end(); it++)
		ham[*it]++;

	if(recalculate)
		CalcProbabilities();
}

void Bayes::LoadDatabases(const char* dbfile)
{
	LoadDatabases(dbfile == NULL ? this->dbfile : string(dbfile));
}

void Bayes::ParseUpdate(istream& contents, time_t* tm, TokenList& ham, TokenList& spam)
{
	string s, count, token;

	string ts;
	getline(contents, ts);
	if(ts.compare(0, 9, "Updated: ") == 0) {
		string tstamp = ts.substr(9);
		*tm = (time_t)(atoi(tstamp.c_str())*3600);
	}

	getline(contents, s);

	if(s.compare("---spam---") == 0) {
		getline(contents, s);
		while(s.compare("---ham---") != 0) {
			int pos = s.find(':');
			token = s.substr(0, pos);
			count = s.substr(pos+1);
			int ct = atoi(count.c_str());
			spam[token] = ct;
			getline(contents, s);
		}

		getline(contents, s);
		while(s.compare("---end---") != 0) {
			int pos = s.find(':');
			token = s.substr(0, pos);
			count = s.substr(pos+1);
			int ct = atoi(count.c_str());
			ham[token] = ct;
			getline(contents, s);
		}
	}
}

void Bayes::LoadDatabases(const string& dbfile)
{
	time(&tm);

	spam.clear();
	ham.clear();

	ifstream ifdb(dbfile.c_str());
	ParseUpdate(ifdb, &tm, ham, spam);
	ifdb.close();

	CalcProbabilities();
}

void Bayes::SaveDatabases(const char* dbfile)
{
	time(&tm);
	SaveDatabases(dbfile == NULL ? this->dbfile : string(dbfile));
}

void Bayes::SaveDatabases(const string& dbfile)
{
	ofstream ofdb(dbfile.c_str(), ios_base::trunc);
	ofdb << "Updated: " << ((tm)/3600) << endl;
	ofdb << "---spam---" << endl;

	for(TokenIter it = spam.begin(); it != spam.end(); it++)
		ofdb << (it->first) << ':' << it->second << endl;

	ofdb << "---ham---" << endl;

	for(TokenIter it = ham.begin(); it != ham.end(); it++)
		ofdb << (it->first) << ':' << it->second << endl;

	ofdb << "---end---" << endl;
	ofdb.close();
}

void Bayes::Merge(TokenList& tokens, BayesTokenType type)
{
	time(&tm);
	TokenList list = type == Spam ? spam : type == Ham ? ham : TokenList();
	for(TokenIter it = tokens.begin(); it != tokens.end(); it++) {
		if(list.count(it->first) == 0)
			list[it->first] = it->second;
		else {
			int diff = it->second - list[it->first];
			list[it->first] += diff;
		}
	}
	CalcProbabilities();
}
