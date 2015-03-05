#pragma once
#include <Windows.h>
#include <cstdlib>
#include <fstream>
#include <map>
#include "Constants.h"
#include "Common.h"
#include "D2Structs.h"

/*
 * MPQInit handles the data we can initialize from MPQ files, provided we
 * are able to load StormLib. It also provides defaults in case we cannot
 * read the MPQ archive.
 */

extern bool USE_CUSTOM_STATS;
extern unsigned int STAT_MAX;

// Item attributes from ItemTypes.txt and Weapon/Armor/Misc.txt
struct ItemAttributes {
	std::string name;
	char code[4];
	std::string category;
	BYTE width;
	BYTE height;
	BYTE stackable;
	BYTE useable;
	BYTE throwable;
	BYTE itemLevel;		// 1=normal, 2=exceptional, 3=elite
	BYTE unusedFlags;
	unsigned int flags;
	unsigned int flags2;
	BYTE qualityLevel;
};

// Item properties from ItemStatCost.txt that we need for parsing incoming 0x9c packets
struct ItemPropertyBits {
	std::string name;
	BYTE saveBits;
	BYTE saveParamBits;
	BYTE saveAdd;
	BYTE op;
	BYTE sendParamBits;
	unsigned short ID;
};

extern std::map<std::string, ItemAttributes*> ItemAttributeMap;
extern std::vector<ItemPropertyBits*> ItemStatList;
extern std::map<std::string, ItemPropertyBits*> ItemStatMap;
extern std::map<std::string, InventoryLayout*> InventoryLayoutMap;


void InitializeMPQData();
