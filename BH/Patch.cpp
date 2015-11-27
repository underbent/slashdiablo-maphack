#include "Patch.h"
std::vector<Patch*> Patch::Patches;

Patch::Patch(PatchType type, Dll dll, int offset, int function, int length) 
: type(type), dll(dll), offset(offset), function(function), length(length) {
	oldCode = new BYTE[length];
	injected = false;
	Patches.push_back(this);
}

int Patch::GetDllOffset(Dll dll, int offset) {
	const char* szDlls[] = {"D2CLIENT.dll", "D2COMMON.dll", "D2GFX.dll", "D2LANG.dll",
							"D2WIN.dll", "D2NET.dll", "D2GAME.dll", "D2LAUNCH.dll",
							"FOG.dll", "BNCLIENT.dll", "STORM.dll", "D2CMP.dll", "D2MULTI.dll", "D2MCPCLIENT.dll", "D2CMP.dll"};
	//Attempt to get the module of the given DLL
	HMODULE hModule = GetModuleHandle(szDlls[dll]);

	//If DLL hasn't been loaded, then load it up!
	if (!hModule) {
		hModule = LoadLibrary(szDlls[dll]);
	}

	//If the DLL isn't there, or failed to load, return.
	if (!hModule)
		return false;

	//Check if it is an ordinal, if so, get the proper address.
	if (offset  < 0)
		return (DWORD)GetProcAddress(hModule, (LPCSTR)(-offset));

	//If just regular offset, add the two and be done!
	return ((DWORD)hModule) + offset;
}

bool Patch::WriteBytes(int address, int len, BYTE* bytes) {
	DWORD dwOld;

	if(!VirtualProtect((void*)address, len, PAGE_READWRITE, &dwOld))
		return FALSE;

	::memcpy((void*)address, bytes, len);
	return !!VirtualProtect((void*)address, len, dwOld, &dwOld);
}

bool Patch::Install() {

	//Check if we have already installed this patch.
	if (IsInstalled())
		return true;

	//Initalize variables for the exactly commands we are injecting.
	BYTE* code = new BYTE[length];
	DWORD protect;

	//Get the proper address that we are patching
	int address = GetDllOffset(dll, offset);

	//Read the old code to allow proper patch removal
	ReadProcessMemory(GetCurrentProcess(), (void*)address, oldCode, length, NULL);
	
	//Set the code with all NOPs by default
	memset(code, 0x90, length);

	if (type != NOP) {
		//Set the JMP or CALL opcode
		code[0] = (type == Call) ? 0xE8 : 0xE9;

		//Set the address to redirect to
		*(DWORD*)&code[1] = function - (address + 5);
	}

	//Write the patch in
	VirtualProtect((VOID*)address, length, PAGE_EXECUTE_READWRITE, &protect);
	memcpy_s((VOID*)address, length, code, length);
	VirtualProtect((VOID*)address, length, protect, &protect);


	//Set that we successfully patched
	injected = true;

	return true;
}

bool Patch::Remove() {
	if (!IsInstalled())
		return true;

	//Get the proper address
	int address = GetDllOffset(dll, offset);
	DWORD protect;

	//Revert to the previous code
	VirtualProtect((VOID*)address, length, PAGE_EXECUTE_READWRITE, &protect);
	memcpy_s((VOID*)address, length, oldCode, length);
	VirtualProtect((VOID*)address, length, protect, &protect);


	injected = false;

	return true;
}