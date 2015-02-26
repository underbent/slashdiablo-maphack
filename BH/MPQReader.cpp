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
		FILE *phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "STARTING A NEW FILE %s\n", file->name.c_str());
		std::stringstream ss(buffer);
		std::string line;
		std::string field;
		if (std::getline(ss, line)) {  // read the header first
			std::stringstream hss(line);
			while (std::getline(hss, field, '\t')) {
				fields.push_back(field);
				fprintf(phil, "Got header field name: %s\n", field.c_str());
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
			fprintf(phil, "Number of data lines: %d\n", data.size());
		}
		fclose(phil);
	}
}
MPQData::~MPQData() {}


bool ReadMPQFiles() {
	FILE *phil;
	phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "phil reading files\n"); fclose(phil);
	int fileCount = 0;
	HMODULE hModule = GetModuleHandle("StormLib.dll");
	if (hModule) {
		phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "phil dll already loaded\n"); fclose(phil);
	}
	//HMODULE dllHandle = LoadLibrary("StormLib.dll");
	HMODULE dllHandle = LoadLibraryEx("StormLib.dll", NULL, LOAD_IGNORE_CODE_AUTHZ_LEVEL);
	phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "here with xhandle %d\n", (int)dllHandle); fclose(phil);
	if (dllHandle) {
		phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "opened dll!\n"); fclose(phil);
		SFileOpenArchive = (MPQOpenArchive)GetProcAddress(dllHandle, "SFileOpenArchive");
		SFileCloseArchive = (MPQCloseArchive)GetProcAddress(dllHandle, "SFileCloseArchive");
		SFileOpenFileEx = (MPQOpenFile)GetProcAddress(dllHandle, "SFileOpenFileEx");
		SFileGetFileSize = (MPQGetSize)GetProcAddress(dllHandle, "SFileGetFileSize");
		SFileReadFile = (MPQReadFile)GetProcAddress(dllHandle, "SFileReadFile");
		SFileCloseFile = (MPQCloseFile)GetProcAddress(dllHandle, "SFileCloseFile");

		if (SFileOpenArchive && SFileCloseArchive && SFileOpenFileEx && SFileCloseFile && SFileGetFileSize && SFileReadFile) {
			HANDLE hMpq = NULL;
			MPQArchive archive("phil.mpq"); // phil fixme phil
			if (archive.error == ERROR_SUCCESS) {
				phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "opened the archive\n"); fclose(phil);
				MPQFile armorFile(&archive, "Armor.txt");
				if (armorFile.error == ERROR_SUCCESS) {
					fileCount++;
					MpqDataMap["armor"] = new MPQData(&armorFile);
				}
				MPQFile weaponsFile(&archive, "Weapons.txt");
				if (weaponsFile.error == ERROR_SUCCESS) {
					fileCount++;
					MpqDataMap["weapons"] = new MPQData(&weaponsFile);
				}
				MPQFile miscFile(&archive, "Misc.txt");
				if (miscFile.error == ERROR_SUCCESS) {
					fileCount++;
					MpqDataMap["misc"] = new MPQData(&miscFile);
				}
				MPQFile itemTypesFile(&archive, "ItemTypes.txt");
				if (itemTypesFile.error == ERROR_SUCCESS) {
					fileCount++;
					MpqDataMap["itemtypes"] = new MPQData(&itemTypesFile);
				}
				phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "file errors %d, %d, %d, %d\n", armorFile.error, weaponsFile.error, miscFile.error, itemTypesFile.error); fclose(phil);
			}
			phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "archive error %d\n", archive.error); fclose(phil);
		}
		FreeLibrary(dllHandle);
	} else {
		phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "couldn't open dll: %d\n", GetLastError()); fclose(phil);
	}
	phil = fopen("C:\\philtest.txt", "a+"); fprintf(phil, "file count is %d\n", fileCount); fclose(phil);
	return fileCount == 4;
}
