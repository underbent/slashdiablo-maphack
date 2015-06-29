#include <fstream>
#include <iterator>
#include <algorithm>
#include "TableReader.h"
#include "BH.h"
#include "MPQReader.h"

std::vector<std::string> tokenize(std::string line){
	std::vector<std::string> vstrings;
	int pIdx = 0, idx = line.find_first_of('\t', pIdx);
	while (idx >= 0){
		if (idx == pIdx){
			vstrings.push_back("");
		}
		else{
			vstrings.push_back(line.substr(pIdx, idx - pIdx));
		}

		pIdx = idx + 1;
		idx = line.find_first_of('\t', pIdx);
	}

	return vstrings;
}

bool TableReader::readTextTable(std::string filePath, Table &table){
	//Open the configuration file
	std::fstream file(BH::path + filePath);
	if (!file.is_open())
		return false;
	std::vector<std::string> headers;

	bool firstLine = true;
	std::string line;
	while (std::getline(file, line)){
		//Ensure we have something in the line now.
		if (line.length() == 0 || line.find_first_not_of('\t') == std::string::npos)
			continue;

		auto vec = tokenize(line);
		if (firstLine){
			headers = vec;
			table.headers = headers;
			firstLine = false;
		}
		else{
			auto entry = new JSONObject;
			for (unsigned int i = 0; i < headers.size(); i++){
				if (vec[i].length() > 0){
					entry->set(headers[i], vec[i]);
				}
			}

			table.addEntry(entry);
		}
	}

	return true;
}

/*
* From stringtable.h
* by Pedro Faria (Jarulf)
*
* With Many thanks to Peter Hatch (Ondo)
*/
enum TblFilePart{
	// File info. Some are not used by program

	// Size info of various sections
	HeaderSize = 0x15,
	ElementSize = 0x02,
	NodeSize = 0x11,

	// Header info location
	CRCOffset = 0x00,      // word
	NumElementsOffset = 0x02,      // word
	HashTableSizeOffset = 0x04,      // dword
	VersionOffset = 0x08,      // byte (always 0)
	StringStartOffset = 0x09,      // dword
	NumLoopsOffset = 0x0D,      // dword
	FileSizeOffset = 0x11,      // dword

	// Element info location
	NodeNumOffset = 0x00,      // word

	// Node info location
	ActiveOffset = 0x00,      // byte
	IdxNbrOffset = 0x01,      // word
	HashValueOffset = 0x03,      // dword
	IdxStringOffset = 0x07,      // dword
	NameStringOffset = 0x0B,      // dword
	NameLenOffset = 0x0F,      // word

	// KeyNums
	StringKeyNum = 0,
	PatchStringKeyNum = 10000,
	ExpansionStringKeyNum = 20000
};

inline
static unsigned char readByte(const char* src, int position){
	return src[position];
}

inline
static unsigned short readUShort(const char* src, int position){
	auto val = *(unsigned short *)&src[position];
	return val;
}

inline
static unsigned int readUInt(const char* src, int position){
	return *(unsigned int *)&src[position];
}

inline
static unsigned int readUInt(std::ifstream &file){
	char buf[4];
	file.read(buf, 4);
	return *(unsigned int *)buf;
}

inline
static int readInt(const char* src, int position){
	return *(int *)&src[position];
}

bool TableReader::readTbl(std::string filePath, Table &table){
	std::ifstream file(BH::path + filePath, std::ifstream::binary);
	if (!file.is_open())
		return false;
	file.seekg(FileSizeOffset);
	auto fileSize = readUInt(file);
	file.seekg(0, std::ios::beg);

	char* buf = (char*)malloc(fileSize * sizeof(char));
	file.read(buf, fileSize * sizeof(char));

	auto numElements = readUShort(buf, NumElementsOffset);
	auto firstNodeLocation = HeaderSize + ElementSize*numElements;
	std::vector<int> nodeNumbers(numElements);
	for (int i = 0; i < numElements; i++){
		int nodeNum = readUShort(buf, HeaderSize + (ElementSize * i));
		nodeNumbers[i] = nodeNum;
 	}
	
	for (int i = 0; i < numElements; i++){
		int nodePos = firstNodeLocation + NodeSize * nodeNumbers[i];
		bool active = readByte(buf, nodePos + ActiveOffset) != 0;
		auto nameStringOffset = readInt(buf, nodePos + NameStringOffset);
		JSONObject *obj = new JSONObject();
		obj->set("idx", i);
		table.addEntry(obj);
		if (active){
			obj->set("value", std::string(&buf[nameStringOffset]));
		}
	}
	delete[] buf;
	return true;
}

bool TableReader::readTable(std::string filePath, Table &table)
{
	//If this is a config with a pre-set of values, can't parse!
	if (filePath.length() == 0)
		return false;

	if (filePath.rfind(".txt") == filePath.length() - 4){
		return readTextTable(filePath, table);
	}
	else if(filePath.rfind(".tbl") == filePath.length() - 4){
		return readTbl(filePath, table);
	}
	else{
		return false;
	}
}

bool TableReader::loadMPQData(std::string archiveName, Table &table)
{
	std::transform(archiveName.begin(), archiveName.end(), archiveName.begin(), ::tolower);
	MPQData* mpq = MpqDataMap[archiveName];
	if (!mpq || mpq->error) return false;
	for (auto iter = mpq->data.begin(); iter != mpq->data.end(); iter++){
		auto entry = *iter;
		JSONObject *obj = new JSONObject();
		for (auto header = mpq->fields.begin(); header != mpq->fields.end(); header++){
			std::string h = *header;
			if (entry[h].length() > 0){
				obj->set(h, entry[h]);
			}
		}
		table.addEntry(obj);
	}

	return true;
}

inline
void Table::addEntry(JSONObject *entry){
	data->add(entry);
}

void Table::removeWhere(std::function<bool(JSONElement*)> predicate){
	data->removeWhere(predicate);
}

Table::Table(std::string filePath) : data(new JSONArray()){
	Table *_this = this;
	TableReader::readTable(filePath, *_this);
}

inline
JSONObject* Table::entryAt(int index){
	return data->getObject(index);
}

bool Table::dump(std::string filePath){
	std::string path = BH::path + filePath;
	std::fstream file1;
	file1.open(path, std::ofstream::out);
	file1.close();
	std::fstream file(path, std::ofstream::out | std::ofstream::trunc);
	if (!file.is_open()){
		return false;
	}

	std::string buffer;
	JSONWriter writer(buffer, SER_OPT_FORMATTED);
	data->serialize(writer);
	file.write(buffer.c_str(), buffer.length());
	return true;
}

JSONObject* Table::findEntry(std::function<bool(JSONObject*)> predicate){
	for (int i = 0; i < data->length(); i++){
		JSONObject *obj = data->getObject(i);
		if (predicate(obj)){
			return obj;
		}
	}
	return nullptr;
}

JSONObject* Table::findEntry(std::string field, std::string value){
	for (int i = 0; i < data->length(); i++){
		JSONObject *obj = data->getObject(i);
		if (obj->getString(field).compare(value) == 0){
			return obj;
		}
	}
	return nullptr;
}

JSONObject* Table::binarySearch(std::string field, int value){
	int len = data->length(), val = 0, idx = len / 2, upper = len, lower = 0;
	if (len > 0){
		do{
			JSONObject *obj = data->getObject(idx);
			val = (int)obj->getNumber(field);
			if (val > value){
				upper = idx;
			}
			else if (val < value){
				lower = idx;
			}
			else{
				return obj;
			}
			idx = ((upper - lower) / 2) + lower;
		} while (val != value && (upper - lower > 0));
	}

	return nullptr;
}

bool Tables::init(false);
Table Tables::ItemStatCost;
Table Tables::ItemTypes;
Table Tables::Properties;
Table Tables::Runewords;
Table Tables::UniqueItems;
Table Tables::SetItems;
Table Tables::Skills;
Table Tables::MagicPrefix;
Table Tables::MagicSuffix;

Table Strings;
Table Expansion;
Table Patch;

bool Tables::initTables(){
	bool success = true;
	if (!init){
		init = true;
		// Add tables here:
		//success &= TableReader::readTable("data\\ItemStatCost.txt", ItemStatCost);		
		//success &= TableReader::readTable("data\\ItemTypes.txt", ItemTypes);		
		//success &= TableReader::readTable("data\\Properties.txt", Properties);		
		//success &= TableReader::readTable("data\\runes.txt", Runewords);		
		//success &= TableReader::readTable("data\\skills.txt", Skills);		
		//success &= TableReader::readTable("data\\MagicPrefix.txt", MagicPrefix);		
		//success &= TableReader::readTable("data\\MagicSuffix.txt", MagicSuffix);		
		//success &= TableReader::readTable("data\\SetItems.txt", SetItems);
		//success &= TableReader::readTable("data\\UniqueItems.txt", UniqueItems);

		success &= TableReader::loadMPQData("itemstatcost", ItemStatCost);
		success &= TableReader::loadMPQData("ItemTypes", ItemTypes);
		success &= TableReader::loadMPQData("Properties", Properties);
		success &= TableReader::loadMPQData("runes", Runewords);
		success &= TableReader::loadMPQData("skills", Skills);
		success &= TableReader::loadMPQData("MagicPrefix", MagicPrefix);
		success &= TableReader::loadMPQData("MagicSuffix", MagicSuffix);
		success &= TableReader::loadMPQData("UniqueItems", UniqueItems);
		success &= TableReader::loadMPQData("SetItems", SetItems);

		UniqueItems.removeWhere([](JSONElement* obj){
			return ((JSONObject*)obj)->getString("index").compare("Expansion") == 0;
		});
		SetItems.removeWhere([](JSONElement* obj){
			return ((JSONObject*)obj)->getString("item").length() == 0;
		});
	}

	return success;
}

std::string Tables::getString(int index){
	JSONObject *obj;
	if (index > ExpansionStringKeyNum){
		obj = Expansion.entryAt(index - ExpansionStringKeyNum);
	}
	else if (index > PatchStringKeyNum){
		obj = Patch.entryAt(index - PatchStringKeyNum);
	}
	else{
		obj = Strings.entryAt(index - StringKeyNum);
	}
	if (obj){
		return obj->getString("value");
	}
	return "";
}

bool Tables::isInitialized(){
	return init;
}

