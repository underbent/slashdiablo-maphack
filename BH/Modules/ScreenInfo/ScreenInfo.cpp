#include "Screeninfo.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"
#include <time.h>

using namespace Drawing;

void ScreenInfo::OnLoad() {
	automapInfo = BH::config->ReadArray("AutomapInfo");
	bhText = new Texthook(Perm, 790, 6, "ÿc4BH v0.1.3 (SlashDiablo Branch)");
	bhText->SetAlignment(Right);
	if (BH::cGuardLoaded) {
		Texthook* cGuardText = new Texthook(Perm, 790, 23, "ÿc4cGuard Loaded");
		cGuardText->SetAlignment(Right);
	}
	gameTimer = GetTickCount();
}

void ScreenInfo::OnGameJoin(const string& name, const string& pass, int diff) {
	gameTimer = GetTickCount();
}

// Right-clicking in the chat console pastes from the clipboard
void ScreenInfo::OnRightClick(bool up, int x, int y, bool* block) {
	if (up)
		return;

	int left = 130, top = 500, width = 540, height = 42;
	if (D2CLIENT_GetUIState(0x05) && x >= left && x <= (left + width) && y >= top && y <= (top + height)) {
		*block = true;

		if (IsClipboardFormatAvailable(CF_TEXT)) {
			OpenClipboard(NULL);
			HGLOBAL glob = GetClipboardData(CF_TEXT);
			size_t size = GlobalSize(glob);
			char* cbtext = (char *)glob;

			std::vector<INPUT> events;
			char buffer[120] = {0};
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILANGUAGE, buffer, sizeof(buffer));
			HKL hKeyboardLayout = LoadKeyboardLayout(buffer, KLF_ACTIVATE);

			for (unsigned int i = 0; i < size-1; i++) {
				INPUT keyEvent = {0};
				const SHORT Vk = VkKeyScanEx(cbtext[i], hKeyboardLayout);
				const UINT VKey = MapVirtualKey(LOBYTE(Vk), 0);

				if (HIBYTE(Vk) == 1) {  // shift key must be pressed
					ZeroMemory(&keyEvent, sizeof(keyEvent));
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
					keyEvent.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
					events.push_back(keyEvent);
				}

				ZeroMemory(&keyEvent, sizeof(keyEvent));
				keyEvent.type = INPUT_KEYBOARD;
				keyEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
				keyEvent.ki.wScan = VKey;
				events.push_back(keyEvent);

				ZeroMemory(&keyEvent, sizeof(keyEvent));
				keyEvent.type = INPUT_KEYBOARD;
				keyEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
				keyEvent.ki.wScan = VKey;
				events.push_back(keyEvent);

				if (HIBYTE(Vk) == 1) {  // release shift key
					ZeroMemory(&keyEvent, sizeof(keyEvent));
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					keyEvent.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
					events.push_back(keyEvent);
				}
			}
			CloseClipboard();

			if(hKeyboardLayout) {
				UnloadKeyboardLayout(hKeyboardLayout);
			}
			int retval = SendInput(events.size(), &events[0], sizeof(INPUT));
		}
	}
}

void ScreenInfo::OnAutomapDraw() {
	GameStructInfo* pInfo = (*p_D2CLIENT_GameInfo);
	BnetData* pData = (*p_D2LAUNCH_BnData);
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	char* szDiff[3] = {"Normal", "Nightmare", "Hell"};
	if (!pInfo || !pData || !pUnit)
		return;
	int y = 6+(BH::cGuardLoaded?16:0);

	char gameTime[20];
	int nTime = ((GetTickCount() - gameTimer) / 1000);
	sprintf_s(gameTime, 20, "%.2d:%.2d:%.2d", nTime/3600, (nTime/60)%60, nTime%60);

	time_t tTime;
	time(&tTime);
	CHAR szTime[128] = "";
	struct tm time;
	localtime_s(&time, &tTime);
	strftime(szTime, sizeof(szTime), "%I:%M:%S %p", &time);

	AutomapReplace automap[] = {
		{"GAMENAME", pData->szGameName},
		{"GAMEPASS", pData->szGamePass},
		{"GAMEIP", pData->szGameIP},
		{"GAMEDIFF", szDiff[D2CLIENT_GetDifficulty()]},
		{"ACCOUNTNAME", pData->szAccountName},
		{"CHARNAME", pUnit->pPlayerData->szName},
		{"LEVEL", UnicodeToAnsi(D2CLIENT_GetLevelName(pUnit->pPath->pRoom1->pRoom2->pLevel->dwLevelNo))},
		{"GAMETIME", gameTime},
		{"REALTIME", szTime}
	};

	for (vector<string>::iterator it = automapInfo.begin(); it < automapInfo.end(); it++) {
		string key;
		key.assign(*it);
		for (int n = 0; n < sizeof(automap) / sizeof(automap[0]); n++) {
			if (key.find("%" + automap[n].key + "%") == string::npos)
				continue;
			if (automap[n].value.length() == 0)
				key = "";
			else
				key = key.replace(key.find("%" + automap[n].key + "%"), automap[n].key.length() + 2, automap[n].value);
		}
		if (key.length() > 0)
			Texthook::Draw(790, (y+=16), Right,0,Gold,"%s", key.c_str());
	}
}