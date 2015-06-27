/*
	cInjector - Class to inject/unload DLLs
    Copyright (C) 2009 Sheppard
	
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
#include "PEStructs.h"

#include <tlhelp32.h>

BOOL cInjector::EnableDebugPriv(VOID) // Abin's function.
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) 
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = sedebugnameValue;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if(AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL)) 
			{
				CloseHandle(hToken);
				return TRUE;
			}
		}
	}

	CloseHandle(hToken);

	return FALSE; 
}

BOOL cInjector::InjectDLL(HWND hwnd, wstring wDLLName) {
	DWORD dwPid;
	GetWindowThreadProcessId(hwnd, &dwPid);
	return cInjector::InjectDLL(dwPid, wDLLName);
}
BOOL cInjector::InjectDLL(DWORD dwPid, wstring wDllName)
{
	HANDLE hThread;
	HANDLE hProc;
	HMODULE hKernel32;
	LPVOID lpLoadLibraryW;
	LPVOID lpRemoteString;

	wstring wPath;
	wPath = BH::wPath;
	wPath += wDllName;

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	if(hProc)
	{
		hKernel32 = LoadLibrary(L"Kernel32.DLL");

		if(hKernel32)
		{
			lpLoadLibraryW = GetProcAddress(hKernel32, "LoadLibraryW");

			if(lpLoadLibraryW)
			{
				lpRemoteString = (LPVOID)VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

				if(lpRemoteString)
				{
					WriteProcessMemory(hProc, lpRemoteString, wPath.c_str(), wPath.size() * 2, NULL);

					hThread = CreateRemoteThread(hProc, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(lpLoadLibraryW), lpRemoteString, NULL, NULL);

					WaitForSingleObject(hThread, INFINITE);

					VirtualFreeEx(hProc, lpRemoteString, 0, MEM_RELEASE);

					FreeLibrary(hKernel32);
					CloseHandle(hProc);
					CloseHandle(hThread);

					// If we're injecting into the wrong version of the D2 client, all the offsets
					// will be wrong and we'll silently fail on the first call into a D2 function
					// (generally D2GFX_GetHwnd in BH::Startup). BH::Startup still returns success
					// however, so check for failed injection by looking for "BH.dll" the game process.
					if (FindInjectedModule(dwPid)) {
						return true;
					}

					printf("WARNING: this maphack will only work with Diablo II client version 1.13C!\n");
					printf("You appear to have a different client version. To learn how to downgrade your\n");
					printf("client, see the Guides & Resources section of the slashdiablo subreddit.\n");
					return false;
				} else {
					printf("VirtualAllocEx() failed with error code %d\n", GetLastError());
				}
			} else {
				printf("GetProcAddress() failed with error code %d\n", GetLastError());
			}

			FreeLibrary(hKernel32);
		} else {
			printf("LoadLibrary() failed with error code %d\n", GetLastError());
		}

		CloseHandle(hProc);
	} else {
		printf("OpenProcess() failed with error code %d\n", GetLastError());
	}

	return FALSE;
}

HMODULE GetRemoteModuleAddress(DWORD dwPid, wstring wDllName){
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	HMODULE address;
	unsigned int i;

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, dwPid);
	if (NULL == hProcess)
		return 0;

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				wstring modPath = szModName;
				if (modPath.compare(modPath.length() - wDllName.length(), wDllName.length(), wDllName) == 0) {
					address = hMods[i];
					break;
				}
			}
		}
	}

	CloseHandle(hProcess);
	return address;
}

BOOL cInjector::RunRemoteProc(HWND hwnd, wstring wDllName, string procName){
	DWORD dwPid;
	GetWindowThreadProcessId(hwnd, &dwPid);
	return cInjector::RunRemoteProc(dwPid, wDllName, procName);
}

BOOL cInjector::RunRemoteProc(DWORD dwPid, wstring wDllName, string procName){
	if (!FindInjectedModule(dwPid)) {
		return false;
	}

	HANDLE hThread;
	HANDLE hProc;
	HMODULE hMod;
	LPVOID procAddress;

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	if (hProc)
	{
		std::string s;
		s.assign(wDllName.begin(), wDllName.end());

		PE::ulong procOffset = PE::GetFunctionOffset(s, procName);

		if (procOffset)
		{
			long MODULE_BASE = reinterpret_cast<long>(GetRemoteModuleAddress(dwPid, wDllName));

			hThread = CreateRemoteThread(hProc, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>((LPVOID)(MODULE_BASE + procOffset)), NULL, NULL, NULL);
			WaitForSingleObject(hThread, INFINITE);

			CloseHandle(hProc);
			CloseHandle(hThread);
			return true;
		}
		else {
			printf("GetProcAddress() failed with error code %d\n", GetLastError());
		}

		CloseHandle(hProc);
	}
	else {
		printf("OpenProcess() failed with error code %d\n", GetLastError());
	}

	return FALSE;
}

BOOL cInjector::UnloadDLL(HWND hwnd, HMODULE hDll) {
	DWORD dwPid;
	GetWindowThreadProcessId(hwnd, &dwPid);
	return cInjector::UnloadDLL(dwPid, hDll);
}
BOOL cInjector::UnloadDLL(DWORD dwPid, HMODULE hDll)
{
	HANDLE hThread;
	HANDLE hProc;
	HMODULE hKernel32;
	LPVOID lpFreeLibrary;

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	if(hProc)
	{
		hKernel32 = LoadLibrary(L"Kernel32.DLL");

		if(hKernel32)
		{
			lpFreeLibrary = GetProcAddress(hKernel32, "FreeLibrary");

			if(lpFreeLibrary)
			{
				hThread = CreateRemoteThread(hProc, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(lpFreeLibrary), hDll, NULL, NULL);

				WaitForSingleObject(hThread, INFINITE);

				CloseHandle(hThread);
				FreeLibrary(hKernel32);
				CloseHandle(hProc);
				return true;
			}

			FreeLibrary(hKernel32);
		}

		CloseHandle(hProc);
	}

	return FALSE;
}

HMODULE cInjector::GetRemoteDll(HWND hwnd, wstring wDllName) {
	DWORD dwPid;
	GetWindowThreadProcessId(hwnd, &dwPid);
	return cInjector::GetRemoteDll(dwPid, wDllName);
}
HMODULE cInjector::GetRemoteDll(DWORD dwPid, wstring wDllName)
{
	MODULEENTRY32W ModEntry;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);

	ModEntry.dwSize = sizeof(MODULEENTRY32W);

	if(Module32First(hSnapshot, &ModEntry))
		do {
			if(!wDllName.compare(ModEntry.szModule))
			{
				CloseHandle(hSnapshot);
				return ModEntry.hModule;
			}

			ModEntry.dwSize = sizeof(MODULEENTRY32);
		} while(Module32Next(hSnapshot, &ModEntry));

	CloseHandle(hSnapshot);

	return NULL;
}

INT cInjector::InjectToProcessByDLL(wstring wProcessDll, wstring wDllName)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 ProcessEntry;
	INT nCount = 0;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if(hSnapshot != INVALID_HANDLE_VALUE)
	{
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

		if(Process32First(hSnapshot, &ProcessEntry))
			do {
				if(cInjector::GetRemoteDll(ProcessEntry.th32ProcessID, wProcessDll))
				{
					HMODULE hDll = cInjector::GetRemoteDll(ProcessEntry.th32ProcessID, wDllName);

					if(!hDll)
						cInjector::InjectDLL(ProcessEntry.th32ProcessID, wDllName);
					else cInjector::UnloadDLL(ProcessEntry.th32ProcessID, hDll);

					nCount++;
				}

				ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
			} while(Process32Next(hSnapshot, &ProcessEntry));
	}

	CloseHandle(hSnapshot);

	return nCount;
}

INT cInjector::InjectToProcess(wstring wProcName, wstring wDllName)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 ProcessEntry;
	INT nCount = 0;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if(hSnapshot != INVALID_HANDLE_VALUE)
	{
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

		if(Process32First(hSnapshot, &ProcessEntry))
			do {
				if(!wProcName.compare(ProcessEntry.szExeFile))
				{
					HMODULE hDll = cInjector::GetRemoteDll(ProcessEntry.th32ProcessID, wDllName);

					if(!hDll)
						cInjector::InjectDLL(ProcessEntry.th32ProcessID, wDllName);
					else cInjector::UnloadDLL(ProcessEntry.th32ProcessID, hDll);

					nCount++;
				}

				ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
			} while(Process32Next(hSnapshot, &ProcessEntry));
	}

	CloseHandle(hSnapshot);

	return nCount;
}

BOOL IsUserAdmin(VOID) {
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup); 
	if(b) 
	{
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
		{
			b = FALSE;
		} 
		FreeSid(AdministratorsGroup); 
	}
	return(b);
}

bool FindInjectedModule(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == NULL)
		return false;

	if(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				char buf[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, szModName, -1, buf, MAX_PATH, "?", NULL);
				string s(buf);
				if (s.compare(s.length() - 6, 6, "BH.dll") == 0) {
					return true;
				}
			}
		}
	}
	CloseHandle(hProcess);
	return false;
}
