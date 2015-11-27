#pragma once
#include "JSONObject.h"
#include <functional>

class TableReader;
class Tables;

class Table {
	friend class TableReader;
	friend class Tables;
private:
	std::unique_ptr<JSONArray> data;
	std::vector<std::string> headers;
	void addEntry(JSONObject *entry);
	void removeWhere(std::function<bool(JSONElement*)> predicate);
public:
	Table() : data(new JSONArray()){}
	Table(std::string filePath);

	JSONObject* findEntry(std::function<bool(JSONObject*)> predicate);
	JSONObject* findEntry(std::string field, std::string value);
	JSONObject* binarySearch(std::string field, int value);
	JSONObject* entryAt(int index);

	bool dump(std::string filePath);
};

class TableReader
{
private:
	static bool readTextTable(std::string filePath, Table &table);
	static bool readTbl(std::string filePath, Table &table);
public:
	static bool readTable(std::string filePath, Table &table);
	static bool loadMPQData(std::string archiveName, Table &table);
};

class Tables {
private:
	static bool init;

	Tables(){}
public:
	static bool initTables();

	static Table ItemStatCost;
	static Table ItemTypes;
	static Table Properties;
	static Table Runewords;
	static Table UniqueItems;
	static Table SetItems;
	static Table Skills;
	static Table MagicPrefix;
	static Table MagicSuffix;
	/*static Table Armor;
	static Table Weapons;
	static Table Misc;*/

	static std::string getString(int index);

	static bool isInitialized();
};
