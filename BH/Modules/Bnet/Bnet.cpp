#include "Bnet.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"

unsigned int Bnet::failToJoin;
std::string Bnet::lastName;
std::string Bnet::lastPass;
std::regex Bnet::reg = std::regex("^(.*?)(\\d+)$");

Patch* nextGame1 = new Patch(Call, D2MULTI, 0x14D29, (int)Bnet::NextGamePatch, 5);
Patch* nextGame2 = new Patch(Call, D2MULTI, 0x14A0B, (int)Bnet::NextGamePatch, 5);
Patch* nextPass1 = new Patch(Call, D2MULTI, 0x14D64, (int)Bnet::NextPassPatch, 5);
Patch* nextPass2 = new Patch(Call, D2MULTI, 0x14A46, (int)Bnet::NextPassPatch, 5);
Patch* ftjPatch = new Patch(Call, D2CLIENT, 0x4363E, (int)FailToJoin_Interception, 6);

void Bnet::OnLoad() {
	LoadConfig();
}

void Bnet::LoadConfig() {
	showLastGame = BH::config->ReadBoolean("Autofill Last Game", true);
	showLastPass = BH::config->ReadBoolean("Autofill Last Password", true);
	nextInstead = BH::config->ReadBoolean("Autofill Next Game", true);
	failToJoin = BH::config->ReadInt("Fail To Join", 4000);

	if (showLastGame) {
		nextGame1->Install();
		nextGame2->Install();
	}

	if (showLastPass) {
		nextPass1->Install();
		nextPass2->Install();
	}

	if (failToJoin > 0 && !D2CLIENT_GetPlayerUnit())
		ftjPatch->Install();
}

void Bnet::OnUnload() {
	nextGame1->Remove();
	nextGame2->Remove();

	nextPass1->Remove();
	nextPass2->Remove();

	ftjPatch->Remove();
}

void Bnet::OnGameJoin(const string& name, const string& pass, int diff) {
	if (name.length() > 0)
		lastName = name;

	if (pass.length() > 0)
		lastPass = pass;
	else
		lastPass = "";
	
	if (nextInstead) {
		std::smatch match;
		if (std::regex_search(Bnet::lastName, match, Bnet::reg) && match.size() == 3) {
			std::string name = match.format("$1");
			if (name.length() != 0) {
				int count = atoi(match.format("$2").c_str());

				//Restart at 1 if the next number would exceed the max game name length of 15
				if (lastName.length() == 15) {
					int maxCountLength = 15 - name.length();
					int countLength = 1;
					int tempCount = count + 1;
					while (tempCount > 9) {
						countLength++;
						tempCount /= 10;
					}
					if (countLength > maxCountLength) {
						count = 1;
					} else {
						count++;
					}
				} else {
					count++;
				}
				char buffer[16];
				sprintf_s(buffer, sizeof(buffer), "%s%d", name.c_str(), count);
				lastName = std::string(buffer);
			}
		}
	}

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

	if (showLastPass) {
		nextPass1->Install();
		nextPass2->Install();
	}
}

VOID __fastcall Bnet::NextGamePatch(Control* box, BOOL (__stdcall *FunCallBack)(Control*, DWORD, DWORD)) {
	if (Bnet::lastName.size() == 0)
		return;

	wchar_t *wszLastGameName = AnsiToUnicode(Bnet::lastName.c_str());

	D2WIN_SetControlText(box, wszLastGameName);
	D2WIN_SelectEditBoxText(box);

	// original code
	D2WIN_SetEditBoxProc(box, FunCallBack);
	delete [] wszLastGameName;
}

VOID __fastcall Bnet::NextPassPatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD)) {
	if (Bnet::lastPass.size() == 0)
		return;
	wchar_t *wszLastPass = AnsiToUnicode(Bnet::lastPass.c_str());
	
	D2WIN_SetControlText(box, wszLastPass);
	
	// original code
	D2WIN_SetEditBoxProc(box, FunCallBack);
	delete[] wszLastPass;
}

void __declspec(naked) FailToJoin_Interception()
{
	__asm
	{
		cmp esi, Bnet::failToJoin;
		ret;
	}
}
