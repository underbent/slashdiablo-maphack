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
#include "EventSink.h"
#include <thread>
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
volatile bool terminate_autoinject = false;
void DoAutoInject();

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
	printf("BH v0.1.7e By McGod\n");
	printf("SlashDiablo Branch: Edited By Deadlock, underbent, and Slayergod13\n");
	printf("Visit http://www.reddit.com/r/slashdiablo for updates!\n");

	printf("\n");
	printf("Command-line parameters:\n");
	printf("\t-o <option number>: set injection option (0 for inject all, etc)\n");
	printf("\t-p: don't pause after injection\n");
	printf("\n");

	if (nOpt < 0) {
		printf("Please choose an option to inject.\n");
		printf("\t0) Inject into All\n");
		printf("\t1) Uninject from All\n");
		printf("\t2) Autoinject\n");

		int nCount = 3;
		for (vector<DiabloWindow*>::iterator window = Windows.begin(); window < Windows.end(); window++) {
			(*window)->MenuMessage(nCount++);
		}
		printf("\n");

		nOpt = _getch() - 48;
	}
	std::thread autoinject_thread;
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
		case 2://Autoinject
			for (vector<DiabloWindow*>::iterator window = Windows.begin(); window < Windows.end(); window++) {
				(*window)->Inject();
			}
			autoinject_thread = thread(DoAutoInject);
			printf("Auto injecting into new instances. Press any key to stop.\n");
			_getch();
			noPause = true;
			terminate_autoinject = true;
			autoinject_thread.join();
		break;

		default://Specific window
			int nWindow = nOpt - 3;
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

// Code taken from MSDN https://msdn.microsoft.com/en-us/library/aa390423%28v=vs.85%29.aspx
// This sets up a COM interface to WMI for notifications on new processes with the image name 'Game.exe'
// The notification is handled in EventSink:Indicate
void DoAutoInject()
{
	HRESULT hres;
	printf("\n");

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		cout << "Failed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;
		return;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM negotiates service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);


	if (FAILED(hres)) {
		cout << "Failed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return;                      // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		cout << "Failed to create IWbemLocator object. "
			<< "Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return;                 // Program has failed.
	}

	// Step 4: ---------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
		);

	if (FAILED(hres)) {
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return;                // Program has failed.
	}



	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return;               // Program has failed.
	}

	// Step 6: -------------------------------------------------
	// Receive event notifications -----------------------------

	// Use an unsecured apartment for security
	IUnsecuredApartment* pUnsecApp = NULL;

	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL,
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
		(void**)&pUnsecApp);

	EventSink* pSink = new EventSink;
	pSink->AddRef();

	IUnknown* pStubUnk = NULL;
	pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

	IWbemObjectSink* pStubSink = NULL;
	pStubUnk->QueryInterface(IID_IWbemObjectSink,
		(void **)&pStubSink);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	hres = pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"),
		_bstr_t("SELECT * "
		"FROM __InstanceCreationEvent WITHIN 1 "
		"WHERE TargetInstance ISA 'Win32_Process' and TargetInstance.Name = 'Game.exe'"),
		WBEM_FLAG_SEND_STATUS,
		NULL,
		pStubSink);

	// Check for errors.
	if (FAILED(hres)) {
		printf("ExecNotificationQueryAsync failed "
			"with = 0x%X\n", hres);
		pSvc->Release();
		pLoc->Release();
		pUnsecApp->Release();
		pStubUnk->Release();
		pSink->Release();
		pStubSink->Release();
		CoUninitialize();
		return;
	}

	// Wait for the event
	while (!terminate_autoinject)
	{
		Sleep(1000);
	}

	hres = pSvc->CancelAsyncCall(pStubSink);

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pUnsecApp->Release();
	pStubUnk->Release();
	pSink->Release();
	pStubSink->Release();
	CoUninitialize();

	return;   // Program successfully completed.

}
