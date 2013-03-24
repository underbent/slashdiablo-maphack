#include "Bnet.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"

unsigned int Bnet::failToJoin;
std::string Bnet::lastName;

Patch* nextGame1 = new Patch(Call, D2MULTI, 0x14D29, (int)Bnet::NextGamePatch, 5);
Patch* nextGame2 = new Patch(Call, D2MULTI, 0x14A0B, (int)Bnet::NextGamePatch, 5);
Patch* ftjPatch = new Patch(Call, D2CLIENT, 0x4363E, (int)FailToJoin_Interception, 6);

void Bnet::OnLoad() {
	showLastGame = BH::config->ReadBoolean("Show Last Game", true);
	failToJoin = BH::config->ReadInt("Fail To Join", 4000);

	if (showLastGame) {
		nextGame1->Install();
		nextGame2->Install();
	}

	if (failToJoin > 0 && !D2CLIENT_GetPlayerUnit())
		ftjPatch->Install();		
}

void Bnet::OnUnload() {
	nextGame1->Remove();
	nextGame2->Remove();

	ftjPatch->Remove();
}

void Bnet::OnGameJoin(const string& name, const string& pass, int diff) {
	if (name.length() > 0)
		lastName = name;
	ftjPatch->Remove();

	nextGame1->Remove();
	nextGame2->Remove();
}

void Bnet::OnGameExit() {
	if (failToJoin > 0)
		ftjPatch->Install();

	if (showLastGame) {
		nextGame1->Install();
		nextGame2->Install();
	}
}

VOID __fastcall Bnet::NextGamePatch(Control* box, BOOL (__stdcall *FunCallBack)(Control*,DWORD,DWORD)) {
	if (Bnet::lastName.size() == 0)
		return;

	wchar_t *wszLastGameName = AnsiToUnicode(Bnet::lastName.c_str());

	D2WIN_SetControlText(box, wszLastGameName);
	D2WIN_SelectEditBoxText(box);
	// original code
	D2WIN_SetEditBoxProc(box, FunCallBack);
	delete [] wszLastGameName;
}

void __declspec(naked) FailToJoin_Interception()
{
	__asm
	{
		cmp esi, Bnet::failToJoin;
		ret;
	}
}