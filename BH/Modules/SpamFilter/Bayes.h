#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <ctime>

typedef std::map<const std::string, int> TokenList;
typedef std::map<const std::string, double> TokenProb;
typedef std::vector<const std::string> Tokens;

enum BayesTokenType { Spam, Ham };

class Bayes
{
private:
	TokenList spam, ham;
	TokenProb prob;
	std::string dbfile;
	static char* delims;
	time_t tm;

	void Tokenize(const std::string& item, Tokens& tokens);
	unsigned int GetTokenCount(const std::string& token, BayesTokenType type);
	void CalcProbabilities(void);
	void CalcTokenProbability(const std::string& token);

public:
	Bayes();
	Bayes(const char* dbfile);
	Bayes(const std::string& dbfile);

	int Categorize(const char* item) { return Categorize(std::string(item)); }
	void AddSpam(const char* item, bool recalculate = true) { AddSpam(std::string(item), recalculate); }
	void AddHam(const char* item, bool recalculate = true) { AddHam(std::string(item), recalculate); }

	int Categorize(const std::string& item);
	void AddSpam(const std::string& item, bool recalculate = true);
	void AddHam(const std::string& item, bool recalculate = true);

	void LoadDatabases(const char* dbfile = NULL);
	void SaveDatabases(const char* dbfile = NULL);

	void LoadDatabases(const std::string& dbfile);
	void SaveDatabases(const std::string& dbfile);

	void ParseUpdate(std::istream& contents, time_t* tm, TokenList& ham, TokenList& spam);
	void Merge(TokenList& tokens, BayesTokenType type);

	int GetSpamTokenCount() { return spam.size(); }
	int GetHamTokenCount() { return ham.size(); }
	time_t GetLastUpdate() { return tm; }
};
