/*
	BH.Injector - Injects BH
    Copyright (C) 2011 McGod
	
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "BH.Injector.h"

/*
#undef _DLL
#undef _UNICODE
#include "StormLib.h"
#define _UNICODE
#define _DLL
*/

typedef bool (*ReadMPQFiles)(std::string patchPath, bool writeFiles);

wstring BH::wPath;
string patchPath;

//EnumWindows callback functions, checks for Diablo II process, builds struct, pushs onto vector.
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	//Get the class Name.
	wchar_t szClassName[1024];
	GetClassName(hwnd, szClassName, 1024);
	//Check if it is Diablo II
	if (!wcscmp(szClassName, L"Diablo II")) {
		char szFileName[1024];
		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(hwnd, &dwProcessId);
		if (dwProcessId) {
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
			if (hProcess) {
				UINT ret = GetModuleFileNameExA(hProcess, NULL, szFileName, 1024);
				patchPath.assign(szFileName);
				size_t start_pos = patchPath.find("Game.exe");
				if (start_pos != std::string::npos) {
					patchPath.replace(start_pos, 8, "Patch_D2.mpq");
				}
			}
		}

		//Convert lParam to vector
		vector<DiabloWindow*>* pVector = (vector<DiabloWindow*>*)lParam;
		//Push class into vector
		pVector->push_back(new DiabloWindow(hwnd));
	}
	return true;
}

char* getCmdOption(char** begin, char** end, const std::string& option) {
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

//Main Function
int main(int argc, const char* argv[]) {
	//Get The Path
	wchar_t wtPath[MAX_PATH] = L"";
	GetCurrentDirectory(sizeof(wtPath), wtPath);

	BH::wPath = wtPath;
	BH::wPath += L"\\";
	
	if (!cInjector::EnableDebugPriv()) {
		printf("\tYou must run this injector as an adminstrator!\nIf you're using Windows Vista or Windows 7, right click and choose \'Run as Adminstrator\'");
		system("PAUSE");
		return 1;
	}

	//Create a vector to hold all the Diablo II Windows
	vector<DiabloWindow*> Windows;

	bool noPause = cmdOptionExists((char**)argv, (char**)argv + argc, "-p");

	int nOpt = -1;
	char *numOpt = getCmdOption((char**)argv, (char**)argv + argc, "-o");
	if (numOpt) {
		string str(numOpt);
		stringstream ss(str);
		if ((ss >> nOpt).fail()) {
			printf("\tCannot convert -o argument to an integer!\n");
			system("PAUSE");
			return 1;
		}
	}

	//Call EnumWindows to push all Diablo windows into vector.
	EnumWindows(EnumWindowsProc, (LPARAM)&Windows);

	//Print intro and the beginning of the menu.
	printf("BH v0.1.6a By McGod\n");
	printf("SlashDiablo Branch: Edited By Deadlock, underbent\n");
	printf("Visit http://www.reddit.com/r/slashdiablo for updates!\n");

	printf("\n");
	printf("Command-line parameters:\n");
	printf("\t-o <option number>: set injection option (0 for inject all, etc)\n");
	printf("\t-p: don't pause after injection\n");
	printf("\n");

	if (Windows.size() == 0) {
		printf("\tNo Windows Found!\n");
		system("PAUSE");
		return 1;
	}
	
	if (nOpt < 0) {
		printf("Please choose an option to inject.\n");
		printf("\t0) Inject into All\n");
		printf("\t1) Uninject from All\n");

		int nCount = 2;
		for (vector<DiabloWindow*>::iterator window = Windows.begin(); window < Windows.end(); window++) {
			(*window)->MenuMessage(nCount++);
		}
		printf("\n");

		nOpt = _getch() - 48;
	}

	switch(nOpt) 
	{
		case 0://Inject into all
			for (vector<DiabloWindow*>::iterator window = Windows.begin(); window < Windows.end(); window++){
				(*window)->Inject();
			}
		break;
		case 1://Unload from all
			for (vector<DiabloWindow*>::iterator window = Windows.begin(); window < Windows.end(); window++)
				(*window)->Unload();
		break;
		default://Specific window
			int nWindow = nOpt - 2;
			if (nWindow < 0 || nWindow >= (int)Windows.size())
				printf("You have chosen an invalid option.\n");
			else 
				if (Windows[nWindow]->IsInjected())
					Windows[nWindow]->Unload();
				else
					Windows[nWindow]->Inject();
		break;
	}
	
	if (!noPause) {
		system("PAUSE");
	}
}
