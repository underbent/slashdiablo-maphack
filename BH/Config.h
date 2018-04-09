#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <map>
#include "Common.h"

using namespace std;

struct Toggle {
	unsigned int toggle;
	bool state;
};

class Config {
	private:
		std::string configName;
		std::map<std::string,std::string> contents;
		vector<pair<string,string>> orderedKeyVals;

	public:
		Config(std::string name) : configName(name) {};
		Config(map<std::string, std::string> content) : contents(content) {};

		//Parse the config file and store results
		bool Parse();
		bool Write();

		std::string GetConfigName();
		void SetConfigName(std::string name);

		//Functions to read values from the configuration
		bool						ReadBoolean	(std::string key, bool value);
		std::string					ReadString	(std::string key, std::string value) { return (contents[key].size() == 0) ? value : contents[key]; };
		int							ReadInt		(std::string key, int value);
		unsigned int				ReadKey		(std::string key, std::string toggle);
		Toggle						ReadToggle	(std::string key, std::string toggle, bool defaultState);
		std::vector<string>			ReadArray	(std::string key);
		map<string, string>			ReadAssoc	(std::string key);
		vector<pair<string,string>>	ReadMapList	(std::string key);

		//Functions to write values to the configuration
		void				WriteBoolean(std::string key, bool value) {};

};
