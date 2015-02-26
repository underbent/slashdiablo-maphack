#pragma once

#include <windows.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>

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
private:
	std::vector<std::string> fields;
	std::vector<std::map<std::string, std::string>> data;
};

bool ReadMPQFiles();

extern std::map<std::string, MPQData*> MpqDataMap;
