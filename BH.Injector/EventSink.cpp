// EventSink.cpp
#include "eventsink.h"
#include "BH.Injector.h"
#include <thread>

ULONG EventSink::AddRef() {
	return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release() {
	LONG lRef = InterlockedDecrement(&m_lRef);
	if (lRef == 0)
		delete this;
	return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid, void** ppv) {
	if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
		*ppv = (IWbemObjectSink *) this;
		AddRef();
		return WBEM_S_NO_ERROR;
	} else return E_NOINTERFACE;
}


extern string patchPath;

void InjectIt(DiabloWindow* dw)
{
	dw->Inject();
}

BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam) {
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	wchar_t szClassName[1024];
	GetClassName(hwnd, szClassName, 1024);
	//Check if it is Diablo II
	if (!wcscmp(szClassName, L"Diablo II")) {
		if (lpdwProcessId == lParam) {
			printf("\nDiablo 2 instance found. Injecting... \n");

			
			char szFileName[1024];
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, lpdwProcessId);
			if (hProcess) {
				UINT ret = GetModuleFileNameExA(hProcess, NULL, szFileName, 1024);
				patchPath.assign(szFileName);
				size_t start_pos = patchPath.find("Game.exe");
				if (start_pos != std::string::npos) {
					patchPath.replace(start_pos, 8, "Patch_D2.mpq");
				}
			}
			DiabloWindow dw = DiabloWindow(hwnd);
			std::thread t(InjectIt, &dw);
			t.join();
			return FALSE;
		}
	}
	return TRUE;
}

long processID = 0;



HRESULT EventSink::Indicate(long lObjectCount,
	IWbemClassObject **apObjArray) {
	HRESULT hr = S_OK;
	_variant_t vtProp;

	for (int i = 0; i < lObjectCount; i++) {

		hr = apObjArray[i]->Get(_bstr_t(L"TargetInstance"), 0, &vtProp, 0, 0);
		if (!FAILED(hr)) {
			IUnknown* str = vtProp;
			hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast< void** >(&apObjArray[i]));
			if (SUCCEEDED(hr)) {
				_variant_t cn;
				hr = apObjArray[i]->Get(L"Handle", 0, &cn, NULL, NULL);
				if (SUCCEEDED(hr)) {
					if ((cn.vt != VT_NULL) && (cn.vt != VT_EMPTY))
					{
						processID = _wtol(cn.bstrVal);
						EnumWindows(EnumWindowsProcMy, processID);
					}			
				}
				VariantClear(&cn);
			}
		}
		VariantClear(&vtProp);

	}

	return WBEM_S_NO_ERROR;
}

HRESULT EventSink::SetStatus(
	/* [in] */ LONG lFlags,
	/* [in] */ HRESULT hResult,
	/* [in] */ BSTR strParam,
	/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	) {
	if (lFlags == WBEM_STATUS_COMPLETE) {
		//printf("Call complete. hResult = 0x%X\n", hResult);
	} else if (lFlags == WBEM_STATUS_PROGRESS) {
		//printf("Call in progress.\n");
	}

	return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp

