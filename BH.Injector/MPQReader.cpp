#include "MPQReader.h"


std::map<std::string, MPQData*> MpqDataMap;

#define SFILE_INVALID_SIZE 0xFFFFFFFF

MPQOpenArchive SFileOpenArchive;
MPQCloseArchive SFileCloseArchive;
MPQOpenFile SFileOpenFileEx;
MPQGetSize SFileGetFileSize;
MPQReadFile SFileReadFile;
MPQCloseFile SFileCloseFile;

MPQArchive::MPQArchive(const char *filename) : name(filename), error(ERROR_SUCCESS) {
	if (!SFileOpenArchive(filename, 0, 0, &hMpq)) {
		error = GetLastError();
	}
}
MPQArchive::~MPQArchive() {
	if (hMpq != NULL) {
		SFileCloseArchive(hMpq);
	}
}
HANDLE MPQArchive::GetHandle() {
	return hMpq;
}


MPQFile::MPQFile(MPQArchive *archive, const char *filename) : name(filename), error(ERROR_SUCCESS) {
	if (!SFileOpenFileEx(archive->GetHandle(), filename, 0, &hMpqFile)) {
		error = GetLastError();
	}
}
MPQFile::~MPQFile() {
	if (hMpqFile != NULL) {
		SFileCloseFile(hMpqFile);
	}
}
HANDLE MPQFile::GetHandle() {
	return hMpqFile;
}


MPQData::MPQData(MPQFile *file) : error(ERROR_SUCCESS) {
	DWORD dwBytes = 1;
	std::string buffer;
	char szBuffer[0x10000];
	while (dwBytes > 0) {
		SFileReadFile(file->GetHandle(), szBuffer, sizeof(szBuffer), &dwBytes, NULL);
		if (dwBytes > 0) {
			buffer.append(szBuffer, dwBytes);
		}
	}

	// TODO: need to remove \r, \n chars here
	if (error == ERROR_SUCCESS) {
		std::stringstream ss(buffer);
		std::string line;
		std::string field;
		if (std::getline(ss, line)) {  // read the header first
			std::stringstream hss(line);
			while (std::getline(hss, field, '\t')) {
				fields.push_back(field);
			}
			while (std::getline(ss, line)) {
				std::map<std::string, std::string> linedata;
				std::stringstream fss(line);
				for (std::vector<std::string>::iterator it = fields.begin(); it != fields.end(); it++) {
					if (!std::getline(fss, field, '\t')) {
						field.clear();
					}
					linedata[(*it)] = field;
				}
				data.push_back(linedata);
			}
		}
	}
}
MPQData::~MPQData() {}


// To handle servers with customized mpq files, try to read Patch_D2.mpq using Stormlib
// (http://www.zezula.net/en/mpq/stormlib.html). We load the StormLib dll with LoadLibrary
// to avoid imposing any run- or compile-time dependencies on the user. If we can't load
// the dll or read the mpq, we will fall back on a hard-coded list of the standard items.
//
// We do all this in the injector and write the info to a temp file because of problems
// calling LoadLibrary in the injected dll.

bool ReadMPQFiles(std::string fileName) {
	int successfulFileCount = 0, desiredFileCount = 0;
	HMODULE dllHandle = LoadLibraryW(L"StormLib.dll");
	if (dllHandle) {
		SFileOpenArchive = (MPQOpenArchive)GetProcAddress(dllHandle, "SFileOpenArchive");
		SFileCloseArchive = (MPQCloseArchive)GetProcAddress(dllHandle, "SFileCloseArchive");
		SFileOpenFileEx = (MPQOpenFile)GetProcAddress(dllHandle, "SFileOpenFileEx");
		SFileGetFileSize = (MPQGetSize)GetProcAddress(dllHandle, "SFileGetFileSize");
		SFileReadFile = (MPQReadFile)GetProcAddress(dllHandle, "SFileReadFile");
		SFileCloseFile = (MPQCloseFile)GetProcAddress(dllHandle, "SFileCloseFile");

		if (SFileOpenArchive && SFileCloseArchive && SFileOpenFileEx && SFileCloseFile && SFileGetFileSize && SFileReadFile) {
			// Copy the MPQ file to avoid sharing access violations
			std::string copyFileName(fileName);
			size_t start_pos = copyFileName.find("Patch_D2.mpq");
			if (start_pos != std::string::npos) {
				copyFileName.replace(start_pos, 12, "Patch_D2.copy.mpq");
			}
			std::ifstream src(fileName.c_str(), std::ios::binary);
			std::ofstream dst(copyFileName.c_str(), std::ios::binary);
			dst << src.rdbuf();
			dst.close();
			src.close();

			MPQArchive archive(copyFileName.c_str());
			if (archive.error == ERROR_SUCCESS) {
				MPQFile armorFile(&archive, "data\\global\\excel\\Armor.txt"); desiredFileCount++;
				if (armorFile.error == ERROR_SUCCESS) {
					successfulFileCount++;
					MpqDataMap["armor"] = new MPQData(&armorFile);
				}
				MPQFile weaponsFile(&archive, "data\\global\\excel\\Weapons.txt"); desiredFileCount++;
				if (weaponsFile.error == ERROR_SUCCESS) {
					successfulFileCount++;
					MpqDataMap["weapons"] = new MPQData(&weaponsFile);
				}
				MPQFile miscFile(&archive, "data\\global\\excel\\Misc.txt"); desiredFileCount++;
				if (miscFile.error == ERROR_SUCCESS) {
					successfulFileCount++;
					MpqDataMap["misc"] = new MPQData(&miscFile);
				}
				MPQFile itemTypesFile(&archive, "data\\global\\excel\\ItemTypes.txt"); desiredFileCount++;
				if (itemTypesFile.error == ERROR_SUCCESS) {
					successfulFileCount++;
					MpqDataMap["itemtypes"] = new MPQData(&itemTypesFile);
				}
				MPQFile itemStatCostFile(&archive, "data\\global\\excel\\ItemStatCost.txt"); desiredFileCount++;
				if (itemStatCostFile.error == ERROR_SUCCESS) {
					successfulFileCount++;
					MpqDataMap["itemstatcost"] = new MPQData(&itemStatCostFile);
				}
				MPQFile inventoryFile(&archive, "data\\global\\excel\\Inventory.txt"); desiredFileCount++;
				if (inventoryFile.error == ERROR_SUCCESS) {
					successfulFileCount++;
					MpqDataMap["inventory"] = new MPQData(&inventoryFile);
				}
			}
		}
		FreeLibrary(dllHandle);
	}

	bool wroteFile = false;
	FILE *tempFP = NULL;
	TCHAR lpTempPathBuffer[2*MAX_PATH];
	DWORD dwRetVal = GetTempPath(2*MAX_PATH, lpTempPathBuffer);
	if (dwRetVal > 0 && (dwRetVal + 18) < (2*MAX_PATH)) {
		wcscat_s(lpTempPathBuffer, L"D2BH_MPQ_temp.txt");
	} else {
		lpTempPathBuffer[0] = 0;
	}
	if (successfulFileCount == desiredFileCount && lpTempPathBuffer) {
		DWORD dwRetVal2 = _wfopen_s(&tempFP, lpTempPathBuffer, L"w");
		if (tempFP && dwRetVal2 == 0) {
			printf("Reading data from MPQ file\n");
			std::map<std::string, std::string> throwableMap;
			std::map<std::string, std::string> bodyLocMap;
			std::map<std::string, std::string> parentMap1;
			std::map<std::string, std::string> parentMap2;
			for (std::vector<std::map<std::string, std::string>>::iterator d = MpqDataMap["itemtypes"]->data.begin(); d < MpqDataMap["itemtypes"]->data.end(); d++) {
				if ((*d)["Code"].length() > 0) {
					//fprintf(test, "ITEMTYPE: %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
					//	(*d)["ItemType"].c_str(), (*d)["Code"].c_str(), (*d)["Equiv1"].c_str(), (*d)["Equiv2"].c_str(),
					//	(*d)["Body"].c_str(), (*d)["BodyLoc1"].c_str(), (*d)["BodyLoc2"].c_str(), (*d)["Rare"].c_str(),
					//	(*d)["Charm"].c_str(), (*d)["Normal"].c_str(), (*d)["Throwable"].c_str());
					throwableMap[(*d)["Code"]] = (*d)["Throwable"];
					bodyLocMap[(*d)["Code"]] = (*d)["BodyLoc1"];
					if ((*d)["Equiv1"].length() > 0) {
						parentMap1[(*d)["Code"]] = (*d)["Equiv1"];
					}
					if ((*d)["Equiv2"].length() > 0) {
						parentMap2[(*d)["Code"]] = (*d)["Equiv2"];
					}
				}
			}
			for (std::vector<std::map<std::string, std::string>>::iterator d = MpqDataMap["armor"]->data.begin(); d < MpqDataMap["armor"]->data.end(); d++) {
				if ((*d)["code"].length() > 0) {
					//fprintf(test, "ARMOR: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
					//	(*d)["name"].c_str(), (*d)["namestr"].c_str(), (*d)["code"].c_str(), (*d)["lArm"].c_str(),
					//	(*d)["rArm"].c_str(), (*d)["Torso"].c_str(), (*d)["Legs"].c_str(), (*d)["rSPad"].c_str(),
					//	(*d)["lSPad"].c_str(), (*d)["HellUpgrade"].c_str(), (*d)["NightmareUpgrade"].c_str(), (*d)["type"].c_str(),
					//	(*d)["normcode"].c_str(), (*d)["ubercode"].c_str(), (*d)["ultracode"].c_str());
					std::set<std::string> ancestorTypes;
					unsigned int flags = ITEM_GROUP_ALLARMOR, flags2 = 0;
					FindAncestorTypes((*d)["type"], ancestorTypes, parentMap1, parentMap2);

					if ((*d)["code"].compare((*d)["ubercode"]) == 0) {
						flags |= ITEM_GROUP_EXCEPTIONAL;
					} else if ((*d)["code"].compare((*d)["ultracode"]) == 0) {
						flags |= ITEM_GROUP_ELITE;
					} else {
						flags |= ITEM_GROUP_NORMAL;
					}
					if (ancestorTypes.find("circ") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_CIRCLET;
					} else if (bodyLocMap[(*d)["type"]].compare("head") == 0) {
						flags |= ITEM_GROUP_HELM;
					} else if (bodyLocMap[(*d)["type"]].compare("tors") == 0) {
						flags |= ITEM_GROUP_ARMOR;
					} else if (bodyLocMap[(*d)["type"]].compare("glov") == 0) {
						flags |= ITEM_GROUP_GLOVES;
					} else if (bodyLocMap[(*d)["type"]].compare("feet") == 0) {
						flags |= ITEM_GROUP_BOOTS;
					} else if (bodyLocMap[(*d)["type"]].compare("belt") == 0) {
						flags |= ITEM_GROUP_BELT;
					} else if (bodyLocMap[(*d)["type"]].compare(1, 3, "arm") == 0 && ancestorTypes.find("shld") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_SHIELD;
					}
					flags = AssignClassFlags((*d)["type"], ancestorTypes, flags);

					fprintf(tempFP, "ARMOR,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s\n",
						(*d)["name"].c_str(), (*d)["code"].c_str(), (*d)["type"].c_str(), (*d)["invwidth"].c_str(),
						(*d)["invheight"].c_str(), (*d)["stackable"].c_str(), (*d)["useable"].c_str(), (*d)["throwable"].c_str(),
						flags, flags2, (*d)["level"].c_str());
				}
			}
			for (std::vector<std::map<std::string, std::string>>::iterator d = MpqDataMap["weapons"]->data.begin(); d < MpqDataMap["weapons"]->data.end(); d++) {
				if ((*d)["code"].length() > 0) {
					//fprintf(test, "WEAPON: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
					//	(*d)["name"].c_str(), (*d)["namestr"].c_str(), (*d)["code"].c_str(), (*d)["type"].c_str(),
					//	(*d)["level"].c_str(), (*d)["wclass"].c_str(), (*d)["missiletype"].c_str(),
					//	(*d)["HellUpgrade"].c_str(), (*d)["NightmareUpgrade"].c_str(),
					//	(*d)["normcode"].c_str(), (*d)["ubercode"].c_str(), (*d)["ultracode"].c_str());
					std::set<std::string> ancestorTypes;
					char stackable = ((*d)["stackable"].length() > 0 ? (*d)["stackable"].at(0) - 48 : 0);
					unsigned int flags = ITEM_GROUP_ALLWEAPON, flags2 = 0;
					FindAncestorTypes((*d)["type"], ancestorTypes, parentMap1, parentMap2);

					if ((*d)["code"].compare((*d)["ubercode"]) == 0) {
						flags |= ITEM_GROUP_EXCEPTIONAL;
					} else if ((*d)["code"].compare((*d)["ultracode"]) == 0) {
						flags |= ITEM_GROUP_ELITE;
					} else {
						flags |= ITEM_GROUP_NORMAL;
					}
					if (ancestorTypes.find("club") != ancestorTypes.end() ||
						ancestorTypes.find("hamm") != ancestorTypes.end() ||
						ancestorTypes.find("mace") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_MACE;
					} else if (ancestorTypes.find("wand") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_WAND;
					} else if (ancestorTypes.find("staf") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_STAFF;
					} else if (ancestorTypes.find("bow") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_BOW;
					} else if (ancestorTypes.find("axe") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_AXE;
					} else if (ancestorTypes.find("scep") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_SCEPTER;
					} else if (ancestorTypes.find("swor") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_SWORD;
					} else if (ancestorTypes.find("knif") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_DAGGER;
					} else if (ancestorTypes.find("spea") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_SPEAR;
					} else if (ancestorTypes.find("pole") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_POLEARM;
					} else if (ancestorTypes.find("xbow") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_CROSSBOW;
					} else if (ancestorTypes.find("jave") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_JAVELIN;
					}
					if (ancestorTypes.find("thro") != ancestorTypes.end()) {
						flags |= ITEM_GROUP_THROWING;
					}
					flags = AssignClassFlags((*d)["type"], ancestorTypes, flags);

					fprintf(tempFP, "WEAP,%s,%s,%s,%s,%s,%d,%s,%s,%d,%d,%s\n",
						(*d)["name"].c_str(), (*d)["code"].c_str(), (*d)["type"].c_str(), (*d)["invwidth"].c_str(),
						(*d)["invheight"].c_str(), stackable, (*d)["useable"].c_str(), throwableMap[(*d)["type"]].c_str(),
						flags, flags2, (*d)["level"].c_str());
				}
			}
			for (std::vector<std::map<std::string, std::string>>::iterator d = MpqDataMap["misc"]->data.begin(); d < MpqDataMap["misc"]->data.end(); d++) {
				if ((*d)["code"].length() > 0) {
					//fprintf(test, "MISC: %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
					//	(*d)["name"].c_str(), (*d)["code"].c_str(), (*d)["type"].c_str(), (*d)["type2"].c_str(),
					//	(*d)["level"].c_str(), (*d)["*name"].c_str(), (*d)["missiletype"].c_str(),
					//	(*d)["HellUpgrade"].c_str(), (*d)["NightmareUpgrade"].c_str());
					std::set<std::string> ancestorTypes;
					unsigned int flags = 0, flags2 = 0;
					FindAncestorTypes((*d)["type"], ancestorTypes, parentMap1, parentMap2);
					FindAncestorTypes((*d)["type2"], ancestorTypes, parentMap1, parentMap2);

					if (ancestorTypes.find("rune") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_RUNE;
					}
					if (ancestorTypes.find("gem0") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_CHIPPED;
					} else if (ancestorTypes.find("gem1") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_FLAWED;
					} else if (ancestorTypes.find("gem2") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_REGULAR;
					} else if (ancestorTypes.find("gem3") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_FLAWLESS;
					} else if (ancestorTypes.find("gem4") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_PERFECT;
					}
					if (ancestorTypes.find("gema") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_AMETHYST;
					} else if (ancestorTypes.find("gemd") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_DIAMOND;
					} else if (ancestorTypes.find("geme") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_EMERALD;
					} else if (ancestorTypes.find("gemr") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_RUBY;
					} else if (ancestorTypes.find("gems") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_SAPPHIRE;
					} else if (ancestorTypes.find("gemt") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_TOPAZ;
					} else if (ancestorTypes.find("gemz") != ancestorTypes.end()) {
						flags2 |= ITEM_GROUP_SKULL;
					}

					fprintf(tempFP, "MISC,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s\n",
						(*d)["name"].c_str(), (*d)["code"].c_str(), (*d)["type"].c_str(), (*d)["invwidth"].c_str(),
						(*d)["invheight"].c_str(), (*d)["stackable"].c_str(), (*d)["useable"].c_str(), "0",
						flags, flags2, (*d)["level"].c_str());
				}
			}
			for (std::vector<std::map<std::string, std::string>>::iterator d = MpqDataMap["itemstatcost"]->data.begin(); d < MpqDataMap["itemstatcost"]->data.end(); d++) {
				if ((*d)["ID"].length() > 0) {
					//fprintf(test, "STAT: %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
					//	(*d)["name"].c_str(), (*d)["code"].c_str(), (*d)["type"].c_str(), (*d)["type2"].c_str(),
					//	(*d)["level"].c_str(), (*d)["*name"].c_str(), (*d)["missiletype"].c_str(),
					//	(*d)["HellUpgrade"].c_str(), (*d)["NightmareUpgrade"].c_str());

					fprintf(tempFP, "STAT,%s,%s,%s,%s,%s,%s,%s\n",
						(*d)["Stat"].c_str(), (*d)["ID"].c_str(), (*d)["Send Param Bits"].c_str(), (*d)["Save Bits"].c_str(),
						(*d)["Save Add"].c_str(), (*d)["Save Param Bits"].c_str(), (*d)["op"].c_str());
				}
			}
			for (std::vector<std::map<std::string, std::string>>::iterator d = MpqDataMap["inventory"]->data.begin(); d < MpqDataMap["inventory"]->data.end(); d++) {
				//fprintf(test, "INV: %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
				//	(*d)["class"].c_str(), (*d)["gridX"].c_str(), (*d)["gridY"].c_str(), (*d)["gridLeft"].c_str(),
				//	(*d)["gridRight"].c_str(), (*d)["gridTop"].c_str(), (*d)["gridBottom"].c_str(), (*d)["gridBoxWidth"].c_str(), (*d)["gridBoxHeight"].c_str());

				fprintf(tempFP, "INV,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
					(*d)["class"].c_str(), (*d)["gridX"].c_str(), (*d)["gridY"].c_str(), (*d)["gridLeft"].c_str(),
					(*d)["gridRight"].c_str(), (*d)["gridTop"].c_str(), (*d)["gridBottom"].c_str(), (*d)["gridBoxWidth"].c_str(), (*d)["gridBoxHeight"].c_str());
			}
			wroteFile = true;
		}
	}
	if (tempFP) {
		fclose(tempFP);
	}
	if (lpTempPathBuffer && !wroteFile) {
		_wremove(lpTempPathBuffer);
	}
	return wroteFile;
}

void FindAncestorTypes(std::string type, std::set<std::string>& ancestors, std::map<std::string, std::string>& map1, std::map<std::string, std::string>& map2) {
	ancestors.insert(type);
	std::map<std::string, std::string>::iterator it1 = map1.find(type);
	if (it1 != map1.end()) {
		FindAncestorTypes(it1->second, ancestors, map1, map2);
	}
	std::map<std::string, std::string>::iterator it2 = map2.find(type);
	if (it2 != map2.end()) {
		FindAncestorTypes(it2->second, ancestors, map1, map2);
	}
}

unsigned int AssignClassFlags(std::string type, std::set<std::string>& ancestors, unsigned int flags) {
	if (ancestors.find("amaz") != ancestors.end()) {
		flags |= ITEM_GROUP_AMAZON_WEAPON;
	} else if (ancestors.find("barb") != ancestors.end()) {
		flags |= ITEM_GROUP_BARBARIAN_HELM;
	} else if (ancestors.find("necr") != ancestors.end()) {
		flags |= ITEM_GROUP_NECROMANCER_SHIELD;
	} else if (ancestors.find("pala") != ancestors.end()) {
		flags |= ITEM_GROUP_PALADIN_SHIELD;
	} else if (ancestors.find("sorc") != ancestors.end()) {
		flags |= ITEM_GROUP_SORCERESS_ORB;
	} else if (ancestors.find("assn") != ancestors.end()) {
		flags |= ITEM_GROUP_ASSASSIN_KATAR;
	} else if (ancestors.find("drui") != ancestors.end()) {
		flags |= ITEM_GROUP_DRUID_PELT;
	}
	return flags;
}
