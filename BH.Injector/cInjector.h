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
#pragma once
#include <string>
#include <Windows.h>
#include <psapi.h>

using namespace std;

class cInjector
{
	public:
		static BOOL EnableDebugPriv(VOID);
		static BOOL InjectDLL(DWORD dwPid, wstring wDLLPath);
		static BOOL InjectDLL(HWND hwnd, wstring wDLLPath);

		static BOOL UnloadDLL(DWORD dwPid, HMODULE hDll);
		static BOOL UnloadDLL(HWND hwnd, HMODULE hDll);

		static INT InjectToProcessByDLL(wstring wProcessDll, wstring wDllName);
		static INT InjectToProcess(wstring wProcName, wstring wDllName);

		static HMODULE GetRemoteDll(DWORD dwPid, wstring wDllName);
		static HMODULE GetRemoteDll(HWND hwnd, wstring wDllName);
};
BOOL IsUserAdmin(VOID);
bool FindInjectedModule(DWORD processID);
