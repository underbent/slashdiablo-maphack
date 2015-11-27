#pragma once
#include <vector>
#include <string>
#include <Windows.h>

class Patch;

enum Dll { D2CLIENT=0,D2COMMON,D2GFX,D2LANG,D2WIN,D2NET,D2GAME,D2LAUNCH,FOG,BNCLIENT, STORM, D2CMP, D2MULTI, D2MCPCLIENT};
enum PatchType { Jump=0, Call, NOP };
class Patch {
	private:
		static std::vector<Patch*> Patches;
		Dll dll;
		PatchType type;
		int offset, length, function;
		BYTE* oldCode;
		bool injected;
	public:
		Patch(PatchType type, Dll dll, int offset, int function, int length);

		bool Install ();
		bool Remove ();

		bool IsInstalled() { return injected; };

		static int GetDllOffset(Dll dll, int offset);
		static bool WriteBytes(int address, int len, BYTE* bytes);
};