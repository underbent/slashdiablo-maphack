#pragma once

#include <windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <vector>

// Duplicated from ItemDisplay.h because VS2010 won't let me link/share headers
#define ITEM_GROUP_HELM					0x00000001
#define ITEM_GROUP_ARMOR				0x00000002
#define ITEM_GROUP_SHIELD				0x00000004
#define ITEM_GROUP_GLOVES				0x00000008
#define ITEM_GROUP_BOOTS				0x00000010
#define ITEM_GROUP_BELT					0x00000020
#define ITEM_GROUP_DRUID_PELT			0x00000040
#define ITEM_GROUP_BARBARIAN_HELM		0x00000080
#define ITEM_GROUP_PALADIN_SHIELD		0x00000100
#define ITEM_GROUP_NECROMANCER_SHIELD	0x00000200
#define ITEM_GROUP_AXE					0x00000400
#define ITEM_GROUP_MACE					0x00000800
#define ITEM_GROUP_SWORD				0x00001000
#define ITEM_GROUP_DAGGER				0x00002000
#define ITEM_GROUP_THROWING				0x00004000
#define ITEM_GROUP_JAVELIN				0x00008000
#define ITEM_GROUP_SPEAR				0x00010000
#define ITEM_GROUP_POLEARM				0x00020000
#define ITEM_GROUP_BOW					0x00040000
#define ITEM_GROUP_CROSSBOW				0x00080000
#define ITEM_GROUP_STAFF				0x00100000
#define ITEM_GROUP_WAND					0x00200000
#define ITEM_GROUP_SCEPTER				0x00400000
#define ITEM_GROUP_ASSASSIN_KATAR		0x00800000
#define ITEM_GROUP_SORCERESS_ORB		0x01000000
#define ITEM_GROUP_AMAZON_WEAPON		0x02000000
#define ITEM_GROUP_NORMAL				0x04000000
#define ITEM_GROUP_EXCEPTIONAL			0x08000000
#define ITEM_GROUP_ELITE				0x10000000
#define ITEM_GROUP_ALLARMOR				0x20000000
#define ITEM_GROUP_ALLWEAPON			0x40000000
#define ITEM_GROUP_CIRCLET				0x80000000

#define ITEM_GROUP_CHIPPED				0x00000001
#define ITEM_GROUP_FLAWED				0x00000002
#define ITEM_GROUP_REGULAR				0x00000004
#define ITEM_GROUP_FLAWLESS				0x00000008
#define ITEM_GROUP_PERFECT				0x00000010
#define ITEM_GROUP_AMETHYST				0x00000020
#define ITEM_GROUP_DIAMOND				0x00000040
#define ITEM_GROUP_EMERALD				0x00000080
#define ITEM_GROUP_RUBY					0x00000100
#define ITEM_GROUP_SAPPHIRE				0x00000200
#define ITEM_GROUP_TOPAZ				0x00000400
#define ITEM_GROUP_SKULL				0x00000800
#define ITEM_GROUP_RUNE					0x00001000


typedef bool (WINAPI *MPQOpenArchive)(const char *, DWORD, DWORD, HANDLE *);
typedef bool (WINAPI *MPQCloseArchive)(HANDLE);
typedef bool (WINAPI *MPQOpenFile)(HANDLE, const char *, DWORD, HANDLE *);
typedef bool (WINAPI *MPQGetSize)(HANDLE, DWORD *);
typedef bool (WINAPI *MPQReadFile)(HANDLE, VOID *, DWORD, DWORD *, LPOVERLAPPED);
typedef bool (WINAPI *MPQCloseFile)(HANDLE);

class MPQArchive {
public:
	std::string name;
	int error;
	MPQArchive(const char *filename);
	~MPQArchive();
	HANDLE GetHandle();
private:
	HANDLE hMpq;
};

class MPQFile {
public:
	std::string name;
	int error;
	MPQFile(MPQArchive *archive, const char *filename);
	~MPQFile();
	HANDLE GetHandle();
private:
	HANDLE hMpqFile;
};

class MPQData {
public:
	int error;
	MPQData(MPQFile *file);
	~MPQData();
	std::vector<std::string> fields;
	std::vector<std::map<std::string, std::string>> data;
private:
};

extern std::map<std::string, MPQData*> MpqDataMap;

extern "C" __declspec(dllexport) bool ReadMPQFiles(std::string fileName);
void FindAncestorTypes(std::string type, std::set<std::string>& ancestors, std::map<std::string, std::string>& map1, std::map<std::string, std::string>& map2);
unsigned int AssignClassFlags(std::string type, std::set<std::string>& ancestors, unsigned int flags);
