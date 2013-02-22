#include "Screeninfo.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"
#include <time.h>

using namespace Drawing;

void ScreenInfo::OnLoad() {
	automapInfo = BH::config->ReadArray("AutomapInfo");
	bhText = new Texthook(Perm, 790, 6, "ÿc4BH v0.1.4 (SlashDiablo Branch)");
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

void ScreenInfo::OnDraw() {
	BnetData* pData = (*p_D2LAUNCH_BnData);
	void *quests = D2CLIENT_GetQuestInfo();
	if (!pData || !quests) {
		return;
	}

	ULONGLONG ticks = GetTickCount64();
	ULONGLONG ms = ticks - packetTicks;
	if (!ReceivedQuestPacket && packetRequests < 6 && ms > 5000) {
		// Ask for quest information from the server; server will respond with packet 0x52.
		// (In case we inject mid-game and miss the packets sent upon game creation/joining)
		BYTE RequestQuestData[1] = {0x40};
		D2NET_SendPacket(1, 0, RequestQuestData);
		packetTicks = ticks;
		packetRequests++;
	}

	// It's a kludge to peek into other modules for config info, but it just seems silly to
	// create a new UI tab for each module with config parameters.
	if ((*BH::MiscToggles)["Quest Drop Warning"].state) {
		char *bossNames[3] = {"Mephisto", "Diablo", "Baal"};
		int xpac = pData->nCharFlags & PLAYER_TYPE_EXPANSION;
		int doneDuriel = D2COMMON_GetQuestFlag2(quests, THE_SEVEN_TOMBS, QFLAG_REWARD_GRANTED);
		int doneMephisto = D2COMMON_GetQuestFlag2(quests, THE_GUARDIAN, QFLAG_REWARD_GRANTED);
		int doneDiablo = D2COMMON_GetQuestFlag2(quests, TERRORS_END, QFLAG_REWARD_GRANTED);
		int doneBaal = D2COMMON_GetQuestFlag2(quests, EVE_OF_DESTRUCTION, QFLAG_REWARD_GRANTED);
		int startedMephisto = D2COMMON_GetQuestFlag2(quests, THE_GUARDIAN, QFLAG_QUEST_STARTED);
		int startedDiablo = D2COMMON_GetQuestFlag2(quests, TERRORS_END, QFLAG_QUEST_STARTED);
		int startedBaal = D2COMMON_GetQuestFlag2(quests, EVE_OF_DESTRUCTION, QFLAG_QUEST_STARTED);

		int warning = -1;
		if (doneDuriel && startedMephisto && !doneMephisto && !MephistoBlocked) {
			warning = 0;
		} else if (doneMephisto && startedDiablo && !doneDiablo && !DiabloBlocked) {
			warning = 1;
		} else if (xpac && doneDiablo && startedBaal && !doneBaal && !BaalBlocked) {
			warning = 2;
		}
		if (warning >= 0) {
			ms = ticks - warningTicks;
			if (ms > 2000) {
				warningTicks = ticks;
			} else if (ms > 500) {
				Texthook::Draw(400, 100, Center, 3, Red, "%s Quest Active", bossNames[warning]);
			}
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

void ScreenInfo::OnGamePacketRecv(BYTE* packet, bool* block) {
	// These packets tell us which quests are blocked for us because they were
	// completed by the player who created the game. Packet 29 is sent at game
	// startup and quest events; likewise with packet 52, but also we can trigger
	// packet 52 by sending the server packet 40.
	switch (packet[0])
	{
	case 0x29:
		{
			// The packet consists of two-byte pairs for each of the 41 quests,
			// starting at the third byte. The high bit of the first byte in the pair
			// (corresponding to QFLAG_QUEST_COMPLETED_BEFORE) is always set when the
			// quest was previously completed. QFLAG_PRIMARY_GOAL_ACHIEVED is often
			// set as well.
			// Packet sent at game start, and upon talking to quest NPCs.
			int packetLen = 97;
			MephistoBlocked = (packet[2 + (THE_GUARDIAN * 2)] & 0x80) > 0;
			DiabloBlocked = (packet[2 + (TERRORS_END * 2)] & 0x80) > 0;
			BaalBlocked = (packet[2 + (EVE_OF_DESTRUCTION * 2)] & 0x80) > 0;
			ReceivedQuestPacket = true;  // fixme: want this here?
			break;

			// TODO: does it get cleared when quest completed while we are in game in different act?
		}
	case 0x52:
		{
			// We have one byte for each of the 41 quests: zero if the quest is blocked,
			// and nonzero if we can complete it.
			// Packet sent upon opening quest log, and after sending 0x40 to server.
			int packetLen = 42;
			MephistoBlocked = packet[1 + THE_GUARDIAN] == 0;
			DiabloBlocked = packet[1 + TERRORS_END] == 0;
			BaalBlocked = packet[1 + EVE_OF_DESTRUCTION] == 0;
			ReceivedQuestPacket = true;
			break;
		}
	default:
		break;
	}
	return;
}

void ScreenInfo::OnGameExit() {
	MephistoBlocked = false;
	DiabloBlocked = false;
	BaalBlocked = false;
	ReceivedQuestPacket = false;
}
