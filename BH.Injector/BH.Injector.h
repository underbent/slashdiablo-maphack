/*
	SpamFilter.Injector - Injects SpamFilter
    Copyright (C) 2009 McGod
	
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

#define DLL_NAME L"BH.dll"

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <wchar.h>
#include <conio.h>
#include <vector>
#include "cInjector.h"

using namespace std;

namespace BH
{
	extern wstring wPath;
};

class DiabloWindow {
	public:
		HWND pHwnd;
		wchar_t szTitle[1024];
		HMODULE pDll;
		DiabloWindow(HWND hwnd) : pHwnd(hwnd) { GetWindowText(pHwnd, szTitle, 1024); pDll = cInjector::GetRemoteDll(hwnd, DLL_NAME); };

		BOOL IsInjected() { return !(pDll == NULL); };

		VOID Inject() {
			if (IsInjected()) {
				return;
			}
			if (cInjector::InjectDLL(pHwnd, DLL_NAME)) {
				wprintf(L"Injected BH into %s(HWND: %X)\n", szTitle, pHwnd);
			} else {
				wprintf(L"Failed to inject %s into %s(HWND: %X)\n", DLL_NAME, szTitle, pHwnd);
			}
		};
		VOID Unload() {
			if (!IsInjected()) {
				return;
			}
			if (cInjector::UnloadDLL(pHwnd, pDll)) {
				wprintf(L"Unloaded BH from %s(HWND: %X)\n", szTitle, pHwnd);
			} else {
				wprintf(L"Failed to unload %s from %s(HWND: %X)\n", DLL_NAME, szTitle, pHwnd);
			}
		};

		VOID MenuMessage(INT nOpt) {
			RECT windowRect;
			GetWindowRect(pHwnd, &windowRect);
			if (!IsInjected()) {
				wprintf(L"\t%d) Inject BH into %s (HWND: %X, pos: %ld,%ld)\n", nOpt, szTitle, pHwnd, windowRect.left, windowRect.top);
			} else {
				wprintf(L"\t%d) Unload from %s (HWND: %X, pos: %ld,%ld)\n", nOpt, szTitle, pHwnd, windowRect.left, windowRect.top);
			}
		};
};