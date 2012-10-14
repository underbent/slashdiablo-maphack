#include "BH.h"
#include <Windows.h>

BOOL WINAPI DllMain(HMODULE instance, DWORD reason, VOID* reserved) {
	switch(reason) {
		case DLL_PROCESS_ATTACH:
			return BH::Startup(instance, reserved);
		break;
		case DLL_PROCESS_DETACH:
			return BH::Shutdown();
		break;
	}
}